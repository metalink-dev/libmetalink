noinst_LTLIBRARIES = libmetalinkcore.la
libmetalinkcore_la_SOURCES =\
	metalink_parser.h\
	metalink_types.c metalink_types.h\
	metalink_error.h\
	metalink_pstm.c metalink_pstm.h\
	metalink_pstate.c metalink_pstate.h\
	metalink_pctrl.c metalink_pctrl.h\
	metalink_parser_common.c metalink_parser_common.h\
	session_data.c session_data.h\
	stack.c stack.h\
	list.c list.h\
	string_buffer.c string_buffer.h

if ENABLE_LIBXML2
libmetalinkcore_la_SOURCES += libxml2_metalink_parser.c
endif

if ENABLE_LIBEXPAT
libmetalinkcore_la_SOURCES += libexpat_metalink_parser.c
endif

AM_CFLAGS = -Wall -D_ISOC99_SOURCE -I${top_srcdir}/lib
AM_CPPFLAGS = @XML_CPPFLAGS@ @LIBEXPAT_CPPFLAGS@
