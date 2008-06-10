check_PROGRAMS = main
main_SOURCES = \
	main.c\
	list_test.c list_test.h\
	metalink_pctrl_test.c metalink_pctrl_test.h\
	metalink_parser_test.c metalink_parser_test.h
main_LDADD = ${top_builddir}/lib/libmetalink.la -lcunit
AM_CFLAGS = -Wall -D_ISOC99_SOURCE -I${top_srcdir}/lib
TESTS = main

EXTRA_DIST = test1.xml