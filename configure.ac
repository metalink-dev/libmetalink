AC_PREREQ(2.61)
AC_INIT([libmetalink], [0.0.3], [t-tujikawa@users.sourceforge.net])
AM_INIT_AUTOMAKE()

# Checks for arguments.
AC_ARG_WITH([libexpat], [  --with-libexpat            use libexpat library if installed. Default: yes], [with_libexpat=$withval], [with_libexpat=yes])
AC_ARG_WITH([libxml2], [  --with-libxml2             use libxml2 library if installed. Default: yes], [with_libxml2=$withval], [with_libxml2=yes])

# Checks for programs
AC_PROG_CC
AM_PROG_LIBTOOL
AC_PROG_RANLIB

# Checks for libraries.


if test "x$with_libexpat" = "xyes"; then
  AM_PATH_LIBEXPAT
fi

if test "x$with_libxml2" = "xyes" && test "x$have_libexpat" != "xyes"; then
   AM_PATH_XML2([2.6.24], [have_libxml2=yes])
   if test "x$have_libxml2" = "xyes"; then
      AC_DEFINE([HAVE_LIBXML2], [1], [Define to 1 if you have libxml2.])
   fi
fi

AM_CONDITIONAL([ENABLE_LIBEXPAT], [test "x$have_libexpat" = "xyes"])
AM_CONDITIONAL([ENABLE_LIBXML2], [test "x$have_libxml2" = "xyes"])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strtol strtoll])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	lib/Makefile
	lib/libmetalink.pc
	lib/metalink/Makefile
	test/Makefile
	doc/Makefile
	m4/Makefile
])
AC_OUTPUT
