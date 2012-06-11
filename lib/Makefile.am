SUBDIRS = metalink
lib_LTLIBRARIES = libmetalink.la
libmetalink_la_SOURCES =
libmetalink_la_LIBADD = \
	metalink/libmetalinkcore.la\
	@XML_LIBS@ @EXPAT_LIBS@
libmetalink_la_LDFLAGS = -no-undefined \
        -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
nobase_include_HEADERS = \
	metalink/metalink_parser.h\
	metalink/metalink_types.h\
	metalink/metalink_error.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmetalink.pc
EXTRA_DIST = libmetalink.pc.in
