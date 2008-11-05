SUBDIRS = metalink
lib_LTLIBRARIES = libmetalink.la
libmetalink_la_SOURCES =
libmetalink_la_LIBADD = \
	metalink/libmetalinkcore.la\
	@XML_LIBS@ @LIBEXPAT_LIBS@
libmetalink_la_LDFLAGS = -version-info 1:0:0
nobase_include_HEADERS = \
	metalink/metalink_parser.h\
	metalink/metalink_types.h\
	metalink/metalink_error.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmetalink.pc
EXTRA_DIST = libmetalink.pc.in
