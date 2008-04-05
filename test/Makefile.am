INCLUDES = -I$(top_srcdir)/lib

testscripts = init.d/bar init.d/foo init.d/barf init.d/dual-headers \
	init.d/mountfs init.d/network

EXTRA_DIST = $(testscripts)
TESTS = tstr tstrarg tdep $(srcdir)/tparse.sh $(srcdir)/tparse-dir.sh \
	tinitd tinitd-list
check_PROGRAMS = tstr tstrarg tdep tparse tparse-dir tinitd tinitd-list

tstr_SOURCES = tstr.c
tstr_LDADD = $(top_builddir)/lib/libinitd.la
tstrarg_SOURCES = tstrarg.c
tstrarg_LDADD = $(top_builddir)/lib/libinitd.la
tdep_SOURCES = tdep.c
tdep_LDADD = $(top_builddir)/lib/libinitd.la
tparse_SOURCES = tparse.c
tparse_LDADD = $(top_builddir)/lib/libinitd.la
tparse_dir_SOURCES = tparse-dir.c
tparse_dir_LDADD = $(top_builddir)/lib/libinitd.la
tinitd_SOURCES = tinitd.c
tinitd_LDADD = $(top_builddir)/lib/libinitd.la
tinitd_list_SOURCES = tinitd-list.c
tinitd_list_LDADD = $(top_builddir)/lib/libinitd.la
