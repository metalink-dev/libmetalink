AM_CPPFLAGS = -Wall -I$(srcdir)/../../lib/includes \
	-I$(builddir)/../../lib/includes
LDADD = $(top_builddir)/lib/libmetalink.la

noinst_PROGRAMS = metalinkcat
metalinkcat_SOURCES = metalinkcat.c

EXTRA_DIST = LibO_3.5.4_Win_x86_install_multi.msi.meta4 \
	ubuntu-12.04-server-amd64.metalink
