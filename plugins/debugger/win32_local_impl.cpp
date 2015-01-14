#ifndef __NT__
#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE
#define CONTROL_C_EXIT                      STATUS_CONTROL_C_EXIT
#define DBG_CONTROL_C                    0x40010005L
#define DBG_CONTROL_BREAK                0x40010008L
#define STATUS_GUARD_PAGE_VIOLATION      0x80000001L
#define STATUS_DATATYPE_MISALIGNMENT     0x80000002L
#define STATUS_BREAKPOINT                0x80000003L
#define STATUS_SINGLE_STEP               0x80000004L
#define STATUS_ACCESS_VIOLATION          0xC0000005L
#define STATUS_IN_PAGE_ERROR             0xC0000006L
#define STATUS_INVALID_HANDLE            0xC0000008L
#define STATUS_NO_MEMORY                 0xC0000017L
#define STATUS_ILLEGAL_INSTRUCTION       0xC000001DL
#define STATUS_NONCONTINUABLE_EXCEPTION  0xC0000025L
#define STATUS_INVALID_DISPOSITION       0xC0000026L
#define STATUS_ARRAY_BOUNDS_EXCEEDED     0xC000008CL
#define STATUS_FLOAT_DENORMAL_OPERAND    0xC000008DL
#define STATUS_FLOAT_DIVIDE_BY_ZERO      0xC000008EL
#define STATUS_FLOAT_INEXACT_RESULT      0xC000008FL
#define STATUS_FLOAT_INVALID_OPERATION   0xC0000090L
#define STATUS_FLOAT_OVERFLOW            0xC0000091L
#define STATUS_FLOAT_STACK_CHECK         0xC0000092L
#define STATUS_FLOAT_UNDERFLOW           0xC0000093L
#define STATUS_INTEGER_DIVIDE_BY_ZERO    0xC0000094L
#define STATUS_INTEGER_OVERFLOW          0xC0000095L
#define STATUS_PRIVILEGED_INSTRUCTION    0xC0000096L
#define STATUS_STACK_OVERFLOW            0xC00000FDL
#define STATUS_CONTROL_C_EXIT            0xC000013AL
#define STATUS_FLOAT_MULTIPLE_FAULTS     0xC00002B4L
#define STATUS_FLOAT_MULTIPLE_TRAPS      0xC00002B5L
#define STATUS_REG_NAT_CONSUMPTION       0xC00002C9L
#define SUCCEEDED(x) (x >= 0)
#define FAILED(x) (x < 0)
#endif

#include <expr.hpp>
#include <loader.hpp>
#include "../../ldr/pe/pe.h"
#include "../../plugins/pdb/common.h"
#include "win32_rpc.h"
#include "rpc_hlp.h"

//--------------------------------------------------------------------------
static const char idc_win32_rdmsr_args[] = { VT_LONG, 0 };
static error_t idaapi idc_win32_rdmsr(idc_value_t *argv, idc_value_t *res)
{
  uint64 value = 0; // shut up the compiler
  uval_t reg = argv[0].num;
#ifdef RPC_CLIENT
  void *out = NULL;
  ssize_t outsize;
  int code = g_dbgmod.send_ioctl(WIN32_IOCTL_RDMSR, &reg, sizeof(reg), &out, &outsize);
  if ( SUCCEEDED(code) && outsize == sizeof(value) )
    value = *(uint64*)out;
  qfree(out);
#else
  int code = g_dbgmod.rdmsr(reg, &value);
#endif
  if ( FAILED(code) )
  {
    res->num = code;
    return set_qerrno(eExecThrow); // read error, raise exception
  }
  res->set_int64(value);
  return eOk;
}

//--------------------------------------------------------------------------
static const char idc_win32_wrmsr_args[] = { VT_LONG, VT_INT64, 0 };
static error_t idaapi idc_win32_wrmsr(idc_value_t *argv, idc_value_t *res)
{
  win32_wrmsr_t msr;
  msr.reg = argv[0].num;
  msr.value = argv[1].i64;
#ifdef RPC_CLIENT
  res->num = g_dbgmod.send_ioctl(WIN32_IOCTL_WRMSR, &msr, sizeof(msr), NULL, NULL);
#else
  res->num = g_dbgmod.wrmsr(msr.reg, msr.value);
#endif
  return eOk;
}

//--------------------------------------------------------------------------
// Installs or uninstalls debugger specific idc functions
static bool register_idc_funcs(bool reg)
{
  static const extfun_t funs[] =
  {
    { IDC_READ_MSR,    idc_win32_rdmsr,     idc_win32_rdmsr_args     },
    { IDC_WRITE_MSR,   idc_win32_wrmsr,     idc_win32_wrmsr_args     },
  };
  for ( int i=0; i < qnumber(funs); i++ )
    if ( !set_idc_func_ex(funs[i].name, reg ? funs[i].fp : NULL, funs[i].args, 0) )
      return false;
  return true;
}

//--------------------------------------------------------------------------
void idaapi rebase_if_required_to(ea_t new_base)
{
  if ( is_miniidb() )
    return;

  netnode penode;
  penode.create(PE_NODE);
  ea_t currentbase = new_base;
  ea_t imagebase = ea_t(penode.altval(PE_ALT_IMAGEBASE)); // loading address (usually pe.imagebase)

  if ( imagebase == 0 )
  {
    warning("AUTOHIDE DATABASE\n"
            "IDA could not automatically determine if the program should be\n"
            "rebased in the database because the database format is too old and\n"
            "doesn't contain enough information.\n"
            "Create a new database if you want automated rebasing to work properly.\n"
            "Note you can always manually rebase the program by using the\n"
            "Edit, Segments, Rebase program command.");
  }
  else if ( imagebase != currentbase )
  {
    rebase_or_warn(imagebase, currentbase);
  }
}

//--------------------------------------------------------------------------
bool read_pe_header(peheader_t *pe)
{
  netnode penode;
  penode.create(PE_NODE);
  return penode.valobj(pe, sizeof(peheader_t)) > 0;
}

#if defined(RPC_CLIENT) && defined(ENABLE_REMOTEPDB)

//--------------------------------------------------------------------------
// handler on IDA: Server -> IDA
static int ioctl_handler(
        rpc_engine_t * /*rpc*/,
        int fn,
        const void *buf,
        size_t size,
        void **poutbuf,
        ssize_t *poutsize)
{
  qnotused(size);
  switch ( fn )
  {
    case WIN32_IOCTL_READFILE:
      {
        const uchar *ptr = (const uchar *)buf;
        const uchar *end = ptr + size;
        uint64 offset        = unpack_dq(&ptr, end);
        uint32 length        = unpack_dd(&ptr, end);
        const char *filename = extract_str(&ptr, end);
        *poutbuf = NULL;
        *poutsize = 0;
        if ( length != 0 )
        {
          FILE *infile = qfopen(filename, "rb");
          if ( infile == NULL )
            return -2;

          void *outbuf = qalloc(length);
          if ( outbuf == NULL )
            return -2;

          qfseek(infile, offset, SEEK_SET);
          int readlen = qfread(infile, outbuf, length);
          qfclose(infile);

          if ( readlen < 0 || readlen > length )
          {
            qfree(outbuf);
            return -2;
          }
          *poutbuf = outbuf;
          *poutsize = readlen;
        }
        return 1;
      }
  }
  return 0;
}
#endif

//--------------------------------------------------------------------------
// Initialize Win32 debugger plugin
static bool win32_init_plugin(void)
{
  // Remote debugger? Then nothing to initialize locally
#ifndef RPC_CLIENT
  if ( !init_subsystem() )
    return false;
#endif
  if ( !netnode::inited() || is_miniidb() || inf.is_snapshot() )
  {
#ifndef __NT__
    // local debugger is available if we are running under Windows
    // for other systems only the remote debugger is available
    if ( !debugger.is_remote() )
      return false;
#endif
  }
  else
  {
    if ( inf.filetype != f_PE )
      return false; // only PE files
    if ( ph.id != TARGET_PROCESSOR )
      return false;

    // find out the pe header
    peheader_t pe;
    if ( !read_pe_header(&pe) )
      return false;

    if ( pe.subsys != PES_UNKNOWN )  // Unknown
    {
#ifdef WINCE_DEBUGGER                // connection to PocketPC device
      // debug only wince applications
      if ( pe.subsys != PES_WINCE )  // Windows CE
        return false;
#else
      // debug only gui or console applications
      if ( pe.subsys != PES_WINGUI && pe.subsys != PES_WINCHAR )
      {
#if defined(RPC_CLIENT) && defined(ENABLE_REMOTEPDB)
        if ( !debugger.is_remote() )
          return false;
        // allow loading to handle remote PDB symbols
#else
        return false;
#endif
      }
#endif
    }
  }
#if defined(RPC_CLIENT) && defined(ENABLE_REMOTEPDB)
  g_dbgmod.set_ioctl_handler(ioctl_handler);
#endif
  return true;
}

//--------------------------------------------------------------------------
inline void win32_term_plugin(void)
{
#ifndef RPC_CLIENT
  term_subsystem();
#endif
}

//----------------------------------------------------------------------------
struct pdb_remote_session_t;
void close_pdb_remote_session(pdb_remote_session_t *)
{
}

#ifndef HAVE_PLUGIN_COMMENTS
//--------------------------------------------------------------------------
static const char comment[] = "Userland win32 debugger plugin";
#endif
