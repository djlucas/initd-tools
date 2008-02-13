noinst_LTLIBRARIES = libstr.la
libstr_la_SOURCES = str.c

noinst_PROGRAMS = test tdep
test_SOURCES = test.c
test_LDADD = libstr.la
tdep_SOURCES = tdep.c dep.c
tdep_LDADD = libstr.la
