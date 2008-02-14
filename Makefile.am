noinst_LTLIBRARIES = libdep.la
libdep_la_SOURCES = str.c dep.c dep-list.c

noinst_PROGRAMS = test tdep
test_SOURCES = test.c
test_LDADD = libdep.la
tdep_SOURCES = tdep.c
tdep_LDADD = libdep.la
