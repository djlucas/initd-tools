AC_INIT([initd-tools],[0.1],[dbn.lists@gmail.com])
AC_CONFIG_AUX_DIR([auto])
AM_INIT_AUTOMAKE([foreign])

AC_CONFIG_HEADERS([config.h])
AC_GNU_SOURCE

AC_DEFUN([AC_LIBTOOL_LANG_CXX_CONFIG], [:])
AC_DEFUN([AC_LIBTOOL_LANG_F77_CONFIG], [:])
AC_DEFUN([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])
AC_PROG_LIBTOOL

if test "x$GCC" = xyes; then
	CFLAGS="$CFLAGS -Wall"
fi

AC_ARG_ENABLE([werror],
	[AS_HELP_STRING([--enable-werror],
	[Error on compiler warnings @<:@default=no@:>@])],,
	[enable_werror=no]
)

if test "x$enable_werror" != xno; then
	if test "x$GCC" = xyes; then
		CFLAGS="$CFLAGS -Werror"
	fi
fi

AC_ARG_WITH([initd-dir],
	[AS_HELP_STRING([--with-initd-dir=DIR],
	[Set the default initd directory @<:@/etc/init.d@:>@])],
	[initd_dir="$withval"],
	[initd_dir=/etc/init.d]
)
AC_DEFINE_UNQUOTED([DEF_INITD_DIR], ["$initd_dir"],
	[Default init.d directory])
AC_SUBST([DEF_INITD_DIR], ["$initd_dir"])

dnl Substituted in manpages
RELDATE=`${DATE-date} -u +'%d %b %Y' 2>/dev/null`
AC_SUBST([RELDATE])

AC_CONFIG_FILES([
Makefile
lib/Makefile
src/Makefile
src/install_initd.8
src/remove_initd.8
test/Makefile
])

AC_CONFIG_COMMANDS([mode],[
chmod +x $srcdir/test/tparse.sh $srcdir/test/tparse-dir.sh
])

AC_OUTPUT
