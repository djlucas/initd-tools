AC_INIT([initd-tools],[0.1],[dbn.lists@gmail.com])
AC_CONFIG_AUX_DIR([auto])
AM_INIT_AUTOMAKE([foreign])

AC_DEFUN([AC_LIBTOOL_LANG_CXX_CONFIG], [:])
AC_DEFUN([AC_LIBTOOL_LANG_F77_CONFIG], [:])
AC_DEFUN([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])
AC_PROG_LIBTOOL

AC_CONFIG_HEADERS([config.h])

if test "x$GCC" = xyes; then
	CFLAGS="$CFLAGS -Wall"
fi

AC_CONFIG_FILES([
Makefile
lib/Makefile
test/Makefile
])

AC_OUTPUT
