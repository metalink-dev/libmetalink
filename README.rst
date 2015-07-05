Libmetalink
===========

Libmetalink is a library to read Metalink XML download description
format. It supports both `Metalink version 3
<http://www.metalinker.org/Metalink_3.0_Spec.pdf>`_ and `Metalink
version 4 (RFC 5854) <https://tools.ietf.org/html/rfc5854>`_.

Requirements
------------

The following packages are needed to build the library:

* pkg-config >= 0.20
* libexpat >= 2.1.0 or libxml2 >= 2.7.8

To build and run the unit test programs, the following packages are
needed:

* cunit >= 2.1

Building from git
-----------------

To build from git, run following commands (you need autoconf)::

    $ ./buildconf
    $ ./configure
    $ make

API
---

Include <metalink/metalink.h> header file for public APIs:

.. code-block:: c

    #include <metalink/metalink.h>

Please note that ``metalink_*_set_*``, ``metalink_*_new`` and
``metalink_*_delete`` functions in <metalink/metalink_types.h> will be
hidden from public API in the future release.  The newly written
application should not use these functions.  The existing applications
are advised to stop using these functions.  If you want to hold the
modified data of Metalink, define application specific data structure,
and make a copy of parsed object.
