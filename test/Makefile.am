INCLUDES = -I$(top_srcdir)/lib

EXTRA_DIST = test-script
TESTS = tstr tstrarg tdep tparse tinitd tinitd-list
check_PROGRAMS = $(TESTS)

tstr_SOURCES = tstr.c
tstr_LDADD = $(top_builddir)/lib/libinitd.la
tstrarg_SOURCES = tstrarg.c
tstrarg_LDADD = $(top_builddir)/lib/libinitd.la
tdep_SOURCES = tdep.c
tdep_LDADD = $(top_builddir)/lib/libinitd.la
tparse_SOURCES = tparse.c
tparse_LDADD = $(top_builddir)/lib/libinitd.la
tinitd_SOURCES = tinitd.c
tinitd_LDADD = $(top_builddir)/lib/libinitd.la
tinitd_list_SOURCES = tinitd-list.c
tinitd_list_LDADD = $(top_builddir)/lib/libinitd.la
