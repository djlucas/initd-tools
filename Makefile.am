SUBDIRS = lib src test

dist_noinst_SCRIPTS = abs2rel.sh

# ChangeLog rules
EXTRA_DIST = ChangeLog
MAINTAINERCLEANFILES = ChangeLog
.PHONY: ChangeLog

ChangeLog:
	@if test ! -d $(top_srcdir)/.git; then \
	  if test $(srcdir) != $(builddir) && \
	    test -f $(top_srcdir)/ChangeLog; then \
	    cp $(srcdir)/ChangeLog $@; \
	  else \
	    touch ChangeLog; \
	  fi; \
	  exit 0; \
	fi; \
	echo git --git-dir=$(top_srcdir)/.git log '>' $@; \
	git --git-dir=$(top_srcdir)/.git log > $@ || touch ChangeLog
