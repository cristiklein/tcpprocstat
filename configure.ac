#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([tcpprocstat], [0.1], [Cristian.Klein@inria.fr])
AM_INIT_AUTOMAKE([-Werror])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AM_PROG_CC_C_O
AC_PROG_CC_STDC
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([dl], [dlsym])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_CHECK_FUNCS([dlopen dlsym memset socket read write recv send])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
