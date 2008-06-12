INCLUDES = -I$(top_srcdir)/lib

sbin_PROGRAMS = install_initd remove_initd

install_initd_SOURCES = install_initd.c util.h util.c
install_initd_LDADD = $(top_builddir)/lib/libinitd.la

remove_initd_SOURCES = remove_initd.c util.h util.c
remove_initd_LDADD = $(top_builddir)/lib/libinitd.la

man_MANS = install_initd.8 remove_initd.8

# /lib/initd symlinks
libinitddir = $(exec_prefix)/lib/initd
install-exec-hook:
	$(MKDIR_P) $(DESTDIR)$(libinitddir)
	rm -f $(DESTDIR)$(libinitddir)/install_initd
	$(LN_S) $(sbindir)/install_initd \
	  $(DESTDIR)$(libinitddir)/install_initd
	rm -f $(DESTDIR)$(libinitddir)/remove_initd
	$(LN_S) $(sbindir)/remove_initd \
	  $(DESTDIR)$(libinitddir)/remove_initd

uninstall-hook:
	rm -f $(DESTDIR)$(libinitddir)/install_initd
	rm -f $(DESTDIR)$(libinitddir)/remove_initd
