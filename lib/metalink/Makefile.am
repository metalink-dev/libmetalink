noinst_LTLIBRARIES = libmetalinkcore.la
libmetalinkcore_la_SOURCES =\
	metalink_config.h\
	metalink_parser.h\
	metalink_types.c metalink_types.h\
	metalink_error.h\
	metalink_pstm.c metalink_pstm.h\
	metalink_pstate.c metalink_pstate.h\
	metalink_pstate_v3.c metalink_pstate_v3.h\
	metalink_pstate_v4.c metalink_pstate_v4.h\
	metalink_pctrl.c metalink_pctrl.h\
	metalink_parser_common.c metalink_parser_common.h\
	metalink_session_data.c metalink_session_data.h\
	metalink_stack.c metalink_stack.h\
	metalink_list.c metalink_list.h\
	metalink_string_buffer.c metalink_string_buffer.h\
	metalink_helper.c metalink_helper.h

if ENABLE_LIBXML2
libmetalinkcore_la_SOURCES += libxml2_metalink_parser.c
endif

if ENABLE_LIBEXPAT
libmetalinkcore_la_SOURCES += libexpat_metalink_parser.c
endif

AM_CFLAGS = -Wall -I${top_srcdir}/lib
AM_CPPFLAGS = @XML_CPPFLAGS@ @EXPAT_CFLAGS@
