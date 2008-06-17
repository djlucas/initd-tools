INCLUDES = -I$(top_srcdir)/lib

libinitd_PROGRAMS = install_initd remove_initd

install_initd_SOURCES = install_initd.c util.h util.c
install_initd_LDADD = $(top_builddir)/lib/libinitd.la

remove_initd_SOURCES = remove_initd.c util.h util.c
remove_initd_LDADD = $(top_builddir)/lib/libinitd.la

man_MANS = install_initd.8 remove_initd.8

# sbindir symlinks
install-exec-hook:
	$(MKDIR_P) $(DESTDIR)$(sbindir)
	@relpath=`$(SHELL) $(top_srcdir)/abs2rel.sh $(libinitddir) $(sbindir)` \
	  || relpath=$(libinitddir); \
	for prog in install_initd remove_initd; do \
	  echo rm -f $(DESTDIR)$(sbindir)/$$prog && \
	  rm -f $(DESTDIR)$(sbindir)/$$prog && \
	  echo $(LN_S) $$relpath/$$prog $(DESTDIR)$(sbindir)/$$prog && \
	  $(LN_S) $$relpath/$$prog $(DESTDIR)$(sbindir)/$$prog \
	  || exit $?; \
	done

uninstall-hook:
	rm -f $(DESTDIR)$(sbindir)/install_initd
	rm -f $(DESTDIR)$(sbindir)/remove_initd
