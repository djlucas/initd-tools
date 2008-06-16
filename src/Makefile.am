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
	@relpath=`$(SHELL) $(top_srcdir)/abs2rel.sh $(sbindir) $(libinitddir)` \
	  || relpath=$(sbindir); \
	for prog in install_initd remove_initd; do \
	  echo rm -f $(DESTDIR)$(libinitddir)/$$prog && \
	  rm -f $(DESTDIR)$(libinitddir)/$$prog && \
	  echo $(LN_S) $$relpath/$$prog $(DESTDIR)$(libinitddir)/$$prog && \
	  $(LN_S) $$relpath/$$prog $(DESTDIR)$(libinitddir)/$$prog \
	  || exit $?; \
	done

uninstall-hook:
	rm -f $(DESTDIR)$(libinitddir)/install_initd
	rm -f $(DESTDIR)$(libinitddir)/remove_initd
