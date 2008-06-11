INCLUDES = -I$(top_srcdir)/lib

sbin_PROGRAMS = install_initd remove_initd

install_initd_SOURCES = install_initd.c util.h util.c
install_initd_LDADD = $(top_builddir)/lib/libinitd.la

remove_initd_SOURCES = remove_initd.c util.h util.c
remove_initd_LDADD = $(top_builddir)/lib/libinitd.la

man_MANS = install_initd.8 remove_initd.8
