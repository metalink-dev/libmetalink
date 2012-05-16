check_PROGRAMS = main
main_SOURCES = \
	main.c\
	metalink_list_test.c metalink_list_test.h\
	metalink_pctrl_test.c metalink_pctrl_test.h\
	metalink_parser_test.c metalink_parser_test.h\
	metalink_parser_test_v4.c metalink_parser_test_v4.h\
	metalink_helper_test.c metalink_helper_test.h
main_LDADD = ${top_builddir}/lib/libmetalink.la -lcunit
main_LDFLAGS = -static
AM_CFLAGS = -Wall -I${top_srcdir}/lib
DEFS += -D_ISOC99_SOURCE -D_GNU_SOURCE
TESTS = main

EXTRA_DIST = test1.xml
