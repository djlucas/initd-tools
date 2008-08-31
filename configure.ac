AC_INIT([initd-tools],[0.1],[dbn.lists@gmail.com])
AC_CONFIG_AUX_DIR([auto])
AM_INIT_AUTOMAKE

dnl Substituted in manpages
RELDATE='31 Aug 2008'
AC_SUBST([RELDATE])

AC_CONFIG_HEADERS([config.h])
AC_GNU_SOURCE

AC_DEFUN([AC_LIBTOOL_LANG_CXX_CONFIG], [:])
AC_DEFUN([AC_LIBTOOL_LANG_F77_CONFIG], [:])
AC_DEFUN([AC_LIBTOOL_LANG_GCJ_CONFIG], [:])
AC_PROG_LIBTOOL
AC_PROG_LN_S

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

# want programs in /usr/lib/initd by default
AC_PREFIX_DEFAULT([/usr])

AC_ARG_WITH([initddir],
	[AS_HELP_STRING([--with-initddir=DIR],
	[Set the default initd directory @<:@/etc/init.d@:>@])],
	[initddir="$withval"],
	[initddir=/etc/init.d]
)
AC_DEFINE_UNQUOTED([DEF_INITD_DIR], ["$initddir"],
	[Default init.d directory])
AC_SUBST([DEF_INITD_DIR], ["$initddir"])

AC_ARG_WITH([initdlibdir],
	[AS_HELP_STRING([--with-initdlibdir=DIR],
	[Set the default initd libdir @<:@${exec_prefix}/lib/initd@:>@])],
	[libinitddir="$withval"],
	[libinitddir='${exec_prefix}/lib/initd']
)
AC_SUBST([libinitddir])

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
