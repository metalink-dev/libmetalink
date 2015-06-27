Libmetalink
===========

Libmetalink is a library to read Metalink XML download description
format. It supports Metalink version 3 and Metalink version 4 (RFC
5854).

Requirements
------------

The following packages are needed to build the library:

* pkg-config >= 0.20
* libexpat >= 2.1.0 or libxml2 >= 2.7.8

To build and run the unit test programs, the following packages are
needed:

cunit >= 2.1

Build from bzr
--------------

To build from bzr, run following commands (you need autoconf)::

    $ autoreconf -i
    $ automake --add-missing
    $ autoconf
    $ ./configure
    $ make

API
---

All public APIs are in metalink/metalink_parser.h,
metalink/metalink_types.h and metalink/metalink_error.h.

Please note that metalink_*_set_*, metalink_*_new and
metalink_*_delete functions in metalink/metalink_types.h will be
hidden from public API in the future release. The newly written
application should not use these functions. The existing applications
are advised to stop using these functions. If you want to hold the
modified data of Metalink, define application specific data structure
for this.
