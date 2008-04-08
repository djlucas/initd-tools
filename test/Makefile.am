INCLUDES = -I$(top_srcdir)/lib

testscripts = init.d/bar init.d/foo init.d/barf init.d/dual-headers \
	init.d/mountfs init.d/network

EXTRA_DIST = $(testscripts)
TESTS = tstr tstrarg tdep $(srcdir)/tparse.sh $(srcdir)/tparse-dir.sh \
	tinitd tinitd-list
check_PROGRAMS = tstr tstrarg tdep tparse tparse-dir tinitd tinitd-list

tstr_SOURCES = tstr.c
tstr_LDADD = $(top_builddir)/lib/libinitd.la
tstrarg_SOURCES = tstrarg.c
tstrarg_LDADD = $(top_builddir)/lib/libinitd.la
tdep_SOURCES = tdep.c
tdep_LDADD = $(top_builddir)/lib/libinitd.la
tparse_SOURCES = tparse.c
tparse_LDADD = $(top_builddir)/lib/libinitd.la
tparse_dir_SOURCES = tparse-dir.c
tparse_dir_LDADD = $(top_builddir)/lib/libinitd.la
tinitd_SOURCES = tinitd.c
tinitd_LDADD = $(top_builddir)/lib/libinitd.la
tinitd_list_SOURCES = tinitd-list.c
tinitd_list_LDADD = $(top_builddir)/lib/libinitd.la

testlinks = rc0.d/K90mountfs rc0.d/K80network \
	rc1.d/S10mountfs rc1.d/K80network \
	rc2.d/S20network rc2.d/S10mountfs \
	rc3.d/S20network rc3.d/S10mountfs \
	rc4.d/S20network rc4.d/S10mountfs \
	rc5.d/S20network rc5.d/S10mountfs \
	rc6.d/K90mountfs rc6.d/K80network
$(TESTS): $(testlinks)
CLEANFILES = $(testlinks)

rctestdirs = rc0.d rc1.d rc2.d rc3.d rc4.d rc5.d rc6.d
clean-local:
	-rm -rf $(rctestdirs)

rc0.d/K90mountfs rc1.d/S10mountfs rc2.d/S10mountfs rc3.d/S10mountfs \
	rc4.d/S10mountfs rc5.d/S10mountfs rc6.d/K90mountfs: init.d/mountfs \
	$(rctestdirs)
	rm -f $@
	ln -s ../$< $@

rc0.d/K80network rc1.d/K80network rc2.d/S20network rc3.d/S20network \
	rc4.d/S20network rc5.d/S20network rc6.d/K80network: init.d/network \
	$(rctestdirs)
	rm -f $@
	ln -s ../$< $@

$(rctestdirs):
	$(MKDIR_P) $@
