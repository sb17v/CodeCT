dnl Process this file with autoconf to produce a configure script.
dnl $Id$

dnl These two lines disable the cache.
define([AC_CACHE_LOAD], )
define([AC_CACHE_SAVE], )

AC_INIT([CodeCT],[0.1])
AC_PREREQ(2.61)
AC_CONFIG_HEADER(codect-config.h)
AC_CONFIG_AUX_DIR(bin)

AC_CANONICAL_TARGET()

AC_PREFIX_DEFAULT(".")
cross_compiling=yes
ac_tool_warned=yes
echo
echo "  Note: codeCT sets cross_compiling to yes to keep configure from failing in"
echo "        the case where test executables would need to be run as a parallel job."
echo


#
#  With specifications
#

AC_ARG_WITH(cc,
	AS_HELP_STRING(--with-cc,Gives the name of the C compiler to use.),
	CC=$withval
)

AC_ARG_WITH(cxx,
	AS_HELP_STRING(--with-cxx,Gives the name of the C++ compiler to use.),
	CXX=$withval
)

AC_ARG_WITH(f77,
	AS_HELP_STRING(--with-f77,Gives the name of the FORTRAN 77 compiler to use.),
	F77=$withval
)

AC_ARG_WITH(include,
	AS_HELP_STRING(--with-include,Specifies include directories (-Idir1 -Idir2).),
	CPPFLAGS=$withval
	echo Using extra include directories: $withval
)

AC_ARG_WITH(ldflags,
	AS_HELP_STRING(--with-ldflags,Specifies ld flags (-L/usr/local/lib).),
	LDFLAGS=$withval
	echo Using ld flags: $withval
)
AC_SUBST(LDFLAGS)

AC_SUBST(CFLAGS)
if test -z "${FFLAGS}" ; then
  FFLAGS=${CFLAGS}
fi
AC_SUBST(FFLAGS)

AC_ARG_WITH(libs,
	AS_HELP_STRING(--with-libs,Specifies libraries (-llib1 -llib2).),
	LIBS=$withval
	echo Using extra libraries: $withval
)

AC_ARG_WITH(binutils-dir,
	AS_HELP_STRING(--with-binutils-dir,Base directory of binutils installation.),
	BINUTILS_DIR=$withval
	CPPFLAGS="${CPPFLAGS} -I${BINUTILS_DIR}/include"
	LIBS="-L${BINUTILS_DIR}/lib ${LIBS}"
	echo Using binutils-dir: $withval
	,
	BINUTILS_DIR=/usr
)
AC_SUBST(BINUTILS_DIR)

AC_ARG_WITH(libunwind,
	AS_HELP_STRING(--with-libunwind,Specify location of libunwind to merge.),
	LIBUNWIND_LOC=$withval
	echo Using libunwind: $withval
	,
	LIBUNWIND_LOC=/usr/lib/libunwind.a
)
AC_SUBST(LIBUNWIND_LOC)

AC_ARG_WITH(bin-type-flag,
	AS_HELP_STRING(--with-bin-type-flag,Binary format for AIX ar operations.),
	BIN_TYPE_FLAG=$withval
	echo Using bin-type-flag: $withval
	,
	BIN_TYPE_FLAG=
)
AC_SUBST(BIN_TYPE_FLAG)

#
#  Enable / Disable specifications
#

DEFAULT_TARGS='${SHARED_C_TARGET}'
ENABLE_API_ONLY=no

ENABLE_BFD=yes
AC_ARG_ENABLE(bfd,
	AS_HELP_STRING(--disable-bfd,Do not use GNU binutils libbfd for source lookup.),
	if test x"$enableval" = xno ; then
		echo Use of libbfd has been disabled.
		ENABLE_BFD=no SO_LOOKUP=no
	fi
	,
)

DISABLE_LIBUNWIND=no
AC_ARG_ENABLE(libunwind,
	AS_HELP_STRING(--disable-libunwind,Do not use libunwind to generate stack traces.),
	if test x"$enableval" = xno ; then
		DISABLE_LIBUNWIND=yes
		echo Use of libunwind has been disabled.
	fi
	,
)

DISABLE_SO_LOOKUP=no
AC_ARG_ENABLE(so-lookup,
	AS_HELP_STRING(--disable-so-lookup,Disable functionality to attempt to find source info in shared objects.),
	if test x"$enableval" = xno ; then
		DISABLE_SO_LOOKUP=yes
		echo Use of shared object source lookup has been disabled.
	fi
	,
)

USE_SETJMP=no
AC_ARG_ENABLE(setjmp,
	AS_HELP_STRING(--enable-setjmp,Use setjmp instead of glic backtrace() to generate stack traces.),
	if test x"$enableval" = xyes ; then
		USE_SETJMP=yes
    AC_DEFINE([USE_SETJMP], [1], [Use setjmp])
		echo Setjmp will be used to generate stack traces.
	fi
	,
)

AC_ARG_ENABLE(fortranxlate,
	AS_HELP_STRING(--disable-fortranxlate,Disable translation of Fortran opaque objects.),
	if test "$enable_fortranxlate" = "no" ; then
		ENABLE_FORTRAN_XLATE=no
		  echo Translation of FORTRAN opaque objects has been disabled.
	else
		ENABLE_FORTRAN_XLATE=yes
		echo Translation of FORTRAN opaque objects has been enabled.
	fi
	,
	ENABLE_FORTRAN_XLATE=yes
)
AC_SUBST(ENABLE_FORTRAN_XLATE)

ENABLE_FORTRAN_WEAK_SYMS=no
AC_ARG_ENABLE(fortranweak,
        AS_HELP_STRING(--enable-fortranweak,Generate weak symbols for additional Fortran symbol name styles.),
        if test "$enable_fortranweak" = "no" ; then
                ENABLE_FORTRAN_WEAK_SYMS=no
                  echo Generation of additional weak FORTRAN symbol names has been disabled.
        else
                ENABLE_FORTRAN_WEAK_SYMS=yes
                AC_DEFINE([ENABLE_FORTRAN_WEAK_SYMS], [1], [Enable weak symbols for FORTRAN])
                echo Generation of additional weak FORTRAN symbol names has been enabled.
        fi
        ,
        ENABLE_FORTRAN_WEAK_SYMS=no
)
AC_SUBST(ENABLE_FORTRAN_WEAK_SYMS)



AC_ARG_ENABLE(stackdepth,
	AS_HELP_STRING([--enable-stackdepth], [Specify maximum report stacktrace depth (default is 8).]),
	STACKDEPTH=$enableval; 
    echo "Maximum stacktrace depth is $STACKDEPTH",
	STACKDEPTH=32,
)
AC_DEFINE_UNQUOTED([CODECT_CALLSITE_REPORT_STACK_DEPTH_MAX], [$STACKDEPTH], [Stack depth of callsites in report])
AC_SUBST(CODECT_CALLSITE_REPORT_STACK_DEPTH_MAX)

AC_ARG_ENABLE(internal-stackdepth,
	AS_HELP_STRING([--enable-internal-stackdepth], [Specify number of internal stack frames (default is 3).]),
	CODECT_INTERNAL_STACK_DEPTH=$enableval; 
    echo "Internal stacktrace depth is $CODECT_INTERNAL_STACK_DEPTH",
	CODECT_INTERNAL_STACK_DEPTH=4,
)
AC_DEFINE_UNQUOTED([CODECT_INTERNAL_STACK_DEPTH], [$CODECT_INTERNAL_STACK_DEPTH], [Number of internal stack frames])
AC_SUBST(CODECT_INTERNAL_STACK_DEPTH)

AC_DEFINE([CODECT_CALLSITE_STACK_DEPTH_MAX], [(CODECT_CALLSITE_REPORT_STACK_DEPTH_MAX + CODECT_INTERNAL_STACK_DEPTH)], [Recorded stack depth of callsites])
AC_SUBST(CODECT_CALLSITE_STACK_DEPTH_MAX)

PC_LOOKUP_FILE=pc_lookup.c
AC_ARG_ENABLE(dwarf,
	AS_HELP_STRING(--enable-dwarf,Use DWARF library for source lookup.),
	if test x"$enableval" = xyes ; then
		echo "Using DWARF library for symbol lookup; forcing --disable-bfd"
		ENABLE_BFD=no
		PC_LOOKUP_FILE=pc_lookup_dwarf.c
	fi ;
	USE_LIBDWARF=yes
	,
	USE_LIBDWARF=no
)
AC_SUBST(USE_LIBDWARF)
AC_SUBST(PC_LOOKUP_FILE)

USE_GETARG=false
AC_ARG_ENABLE(getarg,
	AS_HELP_STRING(--enable-getarg,Use getarg to get fortran command line args.),
	if test x"$enableval" = xyes ; then
		AC_DEFINE([USE_GETARG], [1], [Use getarg])
		echo Use of getarg has been enabled
		USE_GETARG=true
	fi
	,
)
AC_SUBST(USE_GETARG)

AC_ARG_ENABLE(demangling,
	AS_HELP_STRING(--enable-demangling,[Specify GNU, IBM, or Compaq demangling support.]),
	DO_DEMANGLE=$enableval
	echo $DO_DEMANGLE demangling enabled
	,
	DO_DEMANGLE=false
)
AC_SUBST(DO_DEMANGLE)

AC_PROG_INSTALL
AC_PROG_RANLIB
if test "x$AR" = "x"; then
   AR=ar
fi

TEST_LIST=

#  Test for available compilers
if test "$ENABLE_API_ONLY" = no ; then
  AC_CHECK_PROGS(CC, xlc gcc cc icc, cc)
  AC_CHECK_PROGS(CXX, xlC c++ g++ CC cxx icc, c++)
  AC_CHECK_PROGS(F77, xlf f77 g77 pgf77 fort ifc, gfortran)
fi

AC_CHECK_LIB(m,sqrt)

AC_CHECK_SIZEOF(void*)

AC_MSG_CHECKING(PIC flags)
if `$CC -fpic 2>&1 | grep 'warning' >/dev/null 2>&1` ; then
  PIC_FLAGS="-DPIC"
else
  PIC_FLAGS="-fpic -DPIC"
fi
AC_MSG_RESULT($PIC_FLAGS)

SOURCE_LOOKUP=none


if test "x$ENABLE_BFD" = "xyes" ; then
  BFD_AUX_LIBS=
  AC_SEARCH_LIBS([dlopen], [dl dld]) # newer versions of binutils have a libdl dependency
  AC_CHECK_LIB(z,inflate, BFD_AUX_LIBS="$BFD_AUX_LIBS -lz")  # newer versions of binutils have a zlib dependency
  AC_CHECK_LIB(intl,dcgettext__, BFD_AUX_LIBS="$BFD_AUX_LIBS -lintl")  #libintl required only on some platforms, eg AIX
  AC_CHECK_LIB(iberty,objalloc_create, BFD_AUX_LIBS="$BFD_AUX_LIBS -liberty")
  AC_CHECK_HEADER(bfd.h,
    AC_CHECK_LIB(bfd,bfd_openr, LIBS="$LIBS -lbfd", ENABLE_BFD=no, $BFD_AUX_LIBS)
    AC_CHECK_DECL(bfd_get_section_size, AC_DEFINE([HAVE_BFD_GET_SECTION_SIZE], [1], [BFD get section size]), ,[#include "bfd.h"]) 
    AC_CHECK_DECL(bfd_get_section_vma, AC_DEFINE(HAVE_BFD_GET_SECTION_MACROS, [1], [BFD get section macros]), ,[#include "bfd.h"])
    , ENABLE_BFD=no SO_LOOKUP=no
  )
  AC_CHECK_TYPE(bfd_boolean, ,AC_DEFINE([HAVE_BFD_BOOLEAN], [1], [BFD booleans]), [#include "bfd.h"])
  if test "x$ENABLE_BFD" = "xyes" ; then
    AC_CHECK_HEADER(demangle.h, [AC_DEFINE([HAVE_DEMANGLE_H], [1], [Have demangling]) if test "x$DO_DEMANGLE" = xfalse ; then DO_DEMANGLE=GNU ; fi])
    LIBS="$LIBS $BFD_AUX_LIBS"
    AC_DEFINE([ENABLE_BFD], [1], [Enable BFD library for the PC to source lookups])
    SOURCE_LOOKUP=bfd
  fi

else
  if test x$USE_LIBDWARF = xyes ; then
    AC_CHECK_LIB(elf, elf_begin, 
      LIBELF_FOUND=yes, 
      ELFDWARF_ERR="failed to find libelf." ; LIBELF_FOUND=no
      )
    AC_CHECK_LIB(dwarf, dwarf_init, 
      LIBDWARF_FOUND=yes, 
      ELFDWARF_ERR="failed to find libdwarf." ; LIBDWARF_FOUND=no, [-lelf]
      )
    echo $ECHO_N "checking for libelf/libdwarf..." $ECHO_C
    if test x$LIBELF_FOUND = xyes -a x$LIBDWARF_FOUND = xyes ; then
      LIBS="${LIBS} -ldwarf -lelf"
      AC_DEFINE([USE_LIBDWARF], [1], [Use libdwarf])
      echo " found libelf/libdwarf"
      SOURCE_LOOKUP=libelf/libdwarf
      SO_LOOKUP=no
    else
      echo " $ELFDWARF_ERR Not using libelf/libdwarf"
    fi
  fi
fi

UNWIND_WITH=internal

#  Not using AC_CHECK_LIB for libunwind as libunwnd API uses macros in libunwind.h to
#  access platform-specific functions.
HAVE_LIBUNWIND=no
if test "x$DISABLE_LIBUNWIND" = "xno" ; then
	AC_CHECK_HEADER(libunwind.h, AC_DEFINE([HAVE_LIBUNWIND],[1], [Have libunwind]) LIBS="$LIBS -lunwind" HAVE_LIBUNWIND=yes UNWIND_WITH=libunwind, 
	[ if test x$build_cpu = xx86_64 ; then
	  echo
	  echo "  CodeCT on x86_64 platforms requires libunwind."
	  echo "  Please install libunwind and, if necessary, configure CodeCT with "
    echo "  appropriate CFLAGS and LDFLAGS settings."
	  echo
	  exit 1
	fi ]
	)
fi

if test x"$HAVE_LIBUNWIND" = xno && test x"$USE_SETJMP" = xno ; then 
  AC_CHECK_FUNC(backtrace, AC_DEFINE([USE_BACKTRACE], [1], [Use backtrace]) UNWIND_WITH=glibc-backtrace, USE_SETJMP=yes)
fi

if test "x$DISABLE_SO_LOOKUP" = "xno" && test "x$ENABLE_BFD" = xyes; then
  SO_LOOKUP=no
  #  Check for /proc/self/maps, can't use AC_CHECK_FILES since cross compiling
  AC_MSG_CHECKING(/proc/self/maps)
  if test -r /proc/self/maps ; then
    AC_MSG_RESULT([yes])
    SO_LOOKUP=yes
  else
    AC_MSG_RESULT([no])
  fi
  
  #  SO lookup code uses getline, assumes ought to be available if we are using libbfd
  if test "x$SO_LOOKUP" = xyes ; then
    AC_CHECK_FUNC(getline, AC_DEFINE([SO_LOOKUP],[1],[SO lookup]), AC_MSG_RESULT([getline not found so SO lookup is disabled.]))
  fi
fi

if test "$ENABLE_API_ONLY" = no ; then
#  Check for ARM LSE
     AC_MSG_CHECKING(ARM LSE)
     AC_LINK_IFELSE(
       [ AC_LANG_PROGRAM([[#include <stdint.h>]], 
                         [[ #if __aarch64__   
                          int64_t *addr, newval; int64_t ret;

__asm__ __volatile__ ("swpl %2, %0, [%1]\n"
                      : "=&r" (ret)
                      : "r" (addr), "r" (newval)
                      : );
#else
fail
#endif
]] ) ],
	      [AC_MSG_RESULT([yes])] AC_DEFINE(HAVE_ARM_LSE, [1], [Using a processor with LSE]),
              (AC_MSG_RESULT( no)) )
#AC_SUBST(HAVE_ARM_LSE)
fi

if test "$ENABLE_API_ONLY" = no ; then
dnl ========== Find out how FORTRAN symbol names appear in C ==========

AC_MSG_CHECKING(fortran symbols)

if test -n "$F77" -a "$F77" != no ; then
  /bin/rm -f ffunc.f flink.c
  echo "      subroutine f_fun()" > ffunc.f
  echo "      return" >> ffunc.f
  echo "      end" >> ffunc.f
  if ! $F77 $FFLAGS -c ffunc.f 1>/dev/null 2>/dev/null ; then
    AC_MSG_RESULT(Failed to compile fortran test object. Example error follows:)
    F77_OBJ_OUT=`$F77 $FFLAGS -c ffunc.f 2>&1`
    AC_MSG_NOTICE($F77_OBJ_OUT)
    AC_MSG_ERROR([giving up])
  fi
  echo "main(){ FF(); return 0; }" > flink.c
  if $CC -o flink -DFF=f_fun flink.c ffunc.o $LDFLAGS $LIBS 1>/dev/null 2>/dev/null; then
    AC_MSG_RESULT(same as C)
    F77_SYMBOLS=symbol
  elif $CC -o flink -DFF=f_fun_ flink.c ffunc.o $LDFLAGS $LIBS 1>/dev/null 2>/dev/null; then
    AC_MSG_RESULT(lowercase with underscore)
    F77_SYMBOLS=symbol_
  elif $CC -o flink -DFF=f_fun__ flink.c ffunc.o $LDFLAGS $LIBS 1>/dev/null 2>/dev/null; then
    AC_MSG_RESULT(lowercase with 2 underscores)
    F77_SYMBOLS=symbol__
  elif $CC -o flink -DFF=F_FUN flink.c ffunc.o $LDFLAGS $LIBS 1>/dev/null 2>/dev/null; then
    AC_MSG_RESULT(uppercase)
    F77_SYMBOLS=SYMBOL
  elif $CC -o flink -DFF=F_FUN_ flink.c ffunc.o $LDFLAGS $LIBS 1>/dev/null 2>/dev/null; then
    AC_MSG_RESULT(uppercase with underscore)
    F77_SYMBOLS=SYMBOL_
  elif $CC -o flink -DFF=F_FUN_ flink.c ffunc.o $LDFLAGS $LIBS 1>/dev/null 2>/dev/null; then
    AC_MSG_RESULT(uppercase with 2 underscores)
    F77_SYMBOLS=SYMBOL__
  else
    AC_MSG_RESULT(could not determine F77 symbol names. Example error follows:)
    CC_F77_SYM_OUT=`$CC -o flink -DFF=f_fun flink.c ffunc.o $LDFLAGS $LIBS 2>&1`
    AC_MSG_NOTICE($CC_F77_SYM_OUT)
    AC_MSG_ERROR([giving up])
  fi
  /bin/rm -f ffunc.f ffunc.o flink flink.c flink.o ffunc
else
  F77_SYMBOLS=symbol_
  AC_MSG_RESULT(guessing lowercase with underscore)
fi
AC_SUBST(F77_SYMBOLS)
AC_DEFINE_UNQUOTED([F77_SYMBOLS],[$F77_SYMBOLS], [F77 symbols])


dnl ========== Identify F77 ==========

AC_MSG_CHECKING(fortran compiler)
if $F77 -V 2>&1 | grep -e 'Portland Group' 1> /dev/null 2> /dev/null ; then
  F77_VENDOR=PGI
  CODECTFLIB=codeCTpgf
  FORTRAN_FLAG=-DPGI
  FORTRAN_TARGET=libcodeCTpgf.a
elif echo $F77 2>&1 | grep 'xlf' 1> /dev/null 2> /dev/null ; then
  F77_VENDOR=IBM
  if test "x$USE_GETARG" = xtrue ; then
    CODECTFLIB=codeCTxlf
    FORTRAN_TARGET=libcodeCTxlf.a
  else
    CODECTFLIB=codeCT
  fi
elif test "x$G77" = xyes ; then
  F77_VENDOR=GNU
  FORTRAN_FLAG=-DGNU_Fortran
  FORTRAN_TARGET=libcodeCTg77.a
  CODECTFLIB=codeCTg77
elif $F77 -v 2>&1 | grep -e gcc 1> /dev/null 2> /dev/null ; then
  F77_VENDOR=GNU
  FORTRAN_FLAG=-DGNU_Fortran
  FORTRAN_TARGET=libcodeCTg77.a
  CODECTFLIB=codeCTg77
elif $F77 -V 2>&1 | grep -e Intel 1> /dev/null 2> /dev/null ; then
  F77_VENDOR=Intel
  FORTRAN_FLAG=
  FORTRAN_TARGET=
  CODECTFLIB=
elif $F77 -V 2>&1 | grep -e Cray 1>/dev/null 2>/dev/null ; then
  F77_VENDOR=Cray
  FORTRAN_FLAG=-DCray_Fortran
  FORTRAN_TARGET=libcodeCTftn.a
  CODECTFLIB=codeCTftn
elif $F77 -version 2>&1 | grep -e Compaq 1> /dev/null 2> /dev/null ; then
  F77_VENDOR=Compaq
  CODECTFLIB=codeCT
  FORTRAN_FLAG=
  FORTRAN_TARGET=
else
  F77_VENDOR=Unknown
  CODECTFLIB=codeCT
  FORTRAN_FLAG=
  FORTRAN_TARGET=
fi

if test x"$F77_VENDOR" = x"IBM" ; then
  FORTRAN_STR_DECL="character (buf_len) buf"
else
  FORTRAN_STR_DECL="character buf*(*)"
fi

fi #if test "$ENABLE_API_ONLY" = no ; then

AC_MSG_RESULT($F77_VENDOR)
AC_SUBST(F77_VENDOR,$F77_VENDOR)
AC_SUBST(CODECTFLIB,$CODECTFLIB)
AC_SUBST(FORTRAN_FLAG,$FORTRAN_FLAG)
AC_SUBST(FORTRAN_TARGET,$FORTRAN_TARGET)
AC_SUBST(FORTRAN_STR_DECL, $FORTRAN_STR_DECL)
AC_SUBST(ENABLE_BFD, $ENABLE_BFD)
AC_SUBST(ENABLE_API_ONLY)
AC_SUBST(DEFAULT_TARGS)
AC_SUBST(TEST_LIST)
AC_SUBST(USE_BACKTRACE)
AC_SUBST(USE_SETJMP)
AC_SUBST(ADD_OBJS)
AC_SUBST(PIC_FLAGS)
AC_SUBST_FILE(FORTRAN_GETARG)

if test "x${slibdir}" = x ; then
  slibdir=$libdir
fi
AC_SUBST(slibdir)

AC_CONFIG_FILES([Defs.mak Makefile])
AC_OUTPUT

#  
#  Print out Summary of configuration
#

echo
echo "********************************************************************************"
echo "  CodeCT Configuration Summary"
echo
echo "  C compiler               : ${CC}"
echo "  C++ compiler             : ${CXX}"
echo "  Fortran compiler         : ${F77}"
echo
echo "  Stack Unwinding          : ${UNWIND_WITH}"
echo "  Address to Source Lookup : ${SOURCE_LOOKUP}"
echo "********************************************************************************"
