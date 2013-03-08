nodist_bin_SCRIPTS = tcpprocstat
dist_noinst_DATA = tcpprocstat.in
DISTCLEANFILES = tcpprocstat

lib_LTLIBRARIES = libtcpprocstat.la
libtcpprocstat_la_SOURCES = libtcpprocstat.c
libtcpprocstat_la_LIBADD = -ldl

edit = $(SED) \
	-e 's|[@]bindir@|$(bindir)|g' \
	-e 's|[@]libdir@|$(libdir)|g' \
	-e 's|[@]datadir@|$(datadir)|g' \
	-e 's|[@]prefix@|$(prefix)|g'

tcpprocstat: tcpprocstat.in
	$(edit) $< > $@

