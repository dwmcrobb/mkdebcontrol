AC_INIT([mkdebcontrol], [1.0.0], [], [], [http://www.mcplex.net])

dnl  ###  Read some macros.
builtin(include, ./dwm.m4)  dnl

AC_PROG_CC
AC_PROG_CXX
AC_CANONICAL_SYSTEM

DEBARCH=`dpkg --print-architecture`
AC_SUBST(DEBARCH)

DWM_NEED_LIBSTDCPPFS

DWM_SET_PKGVARS(mkdebcontrol,[mkdebcontrol-0.0.0])
DWM_GET_TAG([mkdebcontrol])

AC_CONFIG_FILES([Makefile.vars debcontrol])
AC_OUTPUT
