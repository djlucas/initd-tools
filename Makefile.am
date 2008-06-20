SUBDIRS = lib src test

dist_noinst_SCRIPTS = abs2rel.sh

# ChangeLog rules
EXTRA_DIST = ChangeLog
MAINTAINERCLEANFILES = ChangeLog
.PHONY: ChangeLog

ChangeLog:
	@if test "$(srcdir)" != .; then \
	  echo "Can only create ChangeLog from the source directory"; \
	  exit 0; \
	fi; \
	if test ! -d $(srcdir)/.git; then \
	  echo "No .git directory available. Can't create ChangeLog"; \
	  exit 0; \
	fi; \
	echo git --git-dir=$(srcdir)/.git log '>' $@; \
	git --git-dir=$(srcdir)/.git log > $@
