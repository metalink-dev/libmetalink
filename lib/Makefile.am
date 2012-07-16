SUBDIRS = includes

AM_CFLAGS = -Wall
AM_CPPFLAGS = -I$(srcdir)/includes -I$(builddir)/includes \
	@XML_CPPFLAGS@ @EXPAT_CFLAGS@ \
	@DEFS@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libmetalink.pc
DISTCLEANFILES = $(pkgconfig_DATA)

lib_LTLIBRARIES = libmetalink.la

OBJECTS = \
	metalink_types.c \
	metalink_pstm.c \
	metalink_pstate.c \
	metalink_pstate_v3.c \
	metalink_pstate_v4.c \
	metalink_pctrl.c \
	metalink_parser_common.c \
	metalink_session_data.c \
	metalink_stack.c \
	metalink_list.c \
	metalink_string_buffer.c \
	metalink_helper.c

HFILES = \
	metalink_config.h\
	metalink_pstm.h\
	metalink_pstate.h\
	metalink_pstate_v3.h\
	metalink_pstate_v4.h\
	metalink_pctrl.h\
	metalink_parser_common.h\
	metalink_session_data.h\
	metalink_stack.h\
	metalink_list.h\
	metalink_string_buffer.h\
	metalink_helper.h

if !HAVE_STRPTIME
OBJECTS += strptime.c
HFILES += strptime.h
endif

if !HAVE_TIMEGM
OBJECTS += timegm.c
HFILES += timegm.h
endif

if ENABLE_LIBXML2
OBJECTS += libxml2_metalink_parser.c
endif

if ENABLE_LIBEXPAT
OBJECTS += libexpat_metalink_parser.c
endif

libmetalink_la_SOURCES = $(HFILES) $(OBJECTS)
libmetalink_la_LDFLAGS = -no-undefined \
        -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) \
	@XML_LIBS@ @EXPAT_LIBS@
