noinst_LTLIBRARIES = libinitd.la
libinitd_la_SOURCES = str.c strarg.c dep.c initd.c initd-list.c

noinst_PROGRAMS = tstr tstrarg tdep tinitd tinitd-list
tstr_SOURCES = tstr.c
tstr_LDADD = libinitd.la
tstrarg_SOURCES = tstrarg.c
tstrarg_LDADD = libinitd.la
tdep_SOURCES = tdep.c
tdep_LDADD = libinitd.la
tinitd_SOURCES = tinitd.c
tinitd_LDADD = libinitd.la
tinitd_list_SOURCES = tinitd-list.c
tinitd_list_LDADD = libinitd.la
