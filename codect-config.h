/* codect-config.h.  Generated from codect-config.h.in by configure.  */
/* codect-config.h.in.  Generated from configure.ac by autoheader.  */

/* "Only build API" */
/* #undef ENABLE_API_ONLY */

/* Enable BFD library for the PC to source lookups */
#define ENABLE_BFD 1

/* Enable weak symbols for FORTRAN */
/* #undef ENABLE_FORTRAN_WEAK_SYMS */

/* F77 symbols */
#define F77_SYMBOLS symbol_

/* ARM LSE */
/* #undef HAVE_ARM_LSE */

/* BFD booleans */
/* #undef HAVE_BFD_BOOLEAN */

/* BFD get section size */
/* #undef HAVE_BFD_GET_SECTION_SIZE */

/* BFD macros available before version 2.34 */
/* #undef HAVE_BFD_GET_SECTION_MACROS */

/* Have demangling */
#define HAVE_DEMANGLE_H 1

/* Have libunwind */
#define HAVE_LIBUNWIND 1

/* Stack depth of callsites in report */
#define CODECT_CALLSITE_REPORT_STACK_DEPTH_MAX 32

/* Internal stack frames */
#define CODECT_INTERNAL_STACK_DEPTH 4

/* Recorded stack depth of callsites */
#define CODECT_CALLSITE_STACK_DEPTH_MAX (CODECT_CALLSITE_REPORT_STACK_DEPTH_MAX + CODECT_INTERNAL_STACK_DEPTH)

/* Need Real-time declaration */
/* #undef NEED_MREAD_REAL_TIME_DECL */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "CodeCT"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "CodeCT 0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "codect"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.1"

/* The size of `void*', as computed by sizeof. */
#define SIZEOF_VOIDP 8

/* SO lookup */
#define SO_LOOKUP 1

/* Use backtrace */
/* #undef USE_BACKTRACE */

/* Use getarg */
/* #undef USE_GETARG */

/* Use libdwarf */
/* #undef USE_LIBDWARF */

/* Use setjmp */
/* #undef USE_SETJMP */
