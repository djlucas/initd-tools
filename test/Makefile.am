INCLUDES = -I$(top_srcdir)/lib

runtestscripts = $(srcdir)/tparse.sh $(srcdir)/tparse-dir.sh
testscripts = service/bar service/foo service/barf service/mountfs \
	service/network service/dual-headers
initdtestscripts = init.d/bar init.d/foo init.d/barf init.d/mountfs \
	init.d/network init.d/dual-headers

EXTRA_DIST = $(runtestscripts) $(testscripts)
TESTS = tstr tstrarg tdep $(runtestscripts) tinitd tinitd-list tactive \
	tadd-deps1 tadd-deps2 trem-deps tinstall tremove
check_PROGRAMS = tstr tstrarg tdep tparse tparse-dir tinitd tinitd-list \
	tactive tadd-deps1 tadd-deps2 trem-deps tinstall tremove

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
tactive_SOURCES = tactive.c
tactive_LDADD = $(top_builddir)/lib/libinitd.la
tadd_deps1_SOURCES = tadd-deps1.c
tadd_deps1_LDADD = $(top_builddir)/lib/libinitd.la
tadd_deps2_SOURCES = tadd-deps2.c
tadd_deps2_LDADD = $(top_builddir)/lib/libinitd.la
trem_deps_SOURCES = trem-deps.c
trem_deps_LDADD = $(top_builddir)/lib/libinitd.la
tinstall_SOURCES = tinstall.c
tinstall_LDADD = $(top_builddir)/lib/libinitd.la
tremove_SOURCES = tremove.c
tremove_LDADD = $(top_builddir)/lib/libinitd.la

rctestdirs = rcS.d rc0.d rc1.d rc2.d rc3.d rc4.d rc5.d rc6.d
testmountfslinks = rcS.d/S10mountfs rc0.d/K90mountfs rc6.d/K90mountfs
testnetworklinks = rc0.d/K80network rc1.d/K80network rc2.d/S20network \
	rc3.d/S20network rc4.d/S20network rc5.d/S20network rc6.d/K80network

links: clean-links $(testlinks)
testlinks = $(testmountfslinks) $(testnetworklinks)
$(TESTS): $(initdtestscripts) $(testlinks)

clean-local: clean-initd clean-links
clean-initd:
	-rm -rf init.d
clean-links:
	-rm -rf $(rctestdirs)

init.d:
	mkdir -p $@
$(initdtestscripts): init.d $(testscripts)
	@orig=`echo $@ | sed 's,init.d,service,'` && \
	echo cp -f $(srcdir)/$$orig $@ && \
	cp -f $(srcdir)/$$orig $@

$(testmountfslinks): init.d/mountfs
	@tdir=`dirname $@`; test -d $$tdir || mkdir -p $$tdir
	rm -f $@
	ln -s ../init.d/mountfs $@

$(testnetworklinks): init.d/network
	@tdir=`dirname $@`; test -d $$tdir || mkdir -p $$tdir
	rm -f $@
	ln -s ../init.d/network $@
