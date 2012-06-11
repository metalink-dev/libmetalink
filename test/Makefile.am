if HAVE_CUNIT

check_PROGRAMS = main
main_SOURCES = \
	main.c\
	metalink_list_test.c metalink_list_test.h\
	metalink_pctrl_test.c metalink_pctrl_test.h\
	metalink_parser_test.c metalink_parser_test.h\
	metalink_parser_test_v4.c metalink_parser_test_v4.h\
	metalink_helper_test.c metalink_helper_test.h
main_LDADD = ${top_builddir}/lib/libmetalink.la
main_LDFLAGS = -static  @CUNIT_LIBS@

AM_CFLAGS = -I${top_srcdir}/lib -I${top_srcdir}/lib/includes \
	-I${top_builddir}/lib/includes \
	-DLIBMETALINK_TEST_DIR=\"$(top_srcdir)/test/\" @CUNIT_CFLAGS@
TESTS = main

EXTRA_DIST = test1.xml test2.xml

endif # HAVE_CUNIT
