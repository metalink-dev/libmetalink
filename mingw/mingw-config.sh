# Define _USE_32BIT_TIME_T because 32bit library of MinGW-w64 does not
# implement many 64bit version function.
export ac_cv_func_malloc_0_nonnull=yes
export ac_cv_func_realloc_0_nonnull=yes
HOST=i686-w64-mingw32
./configure \
    --prefix=/usr/local/$HOST \
    --host=$HOST \
    --build=`dpkg-architecture -qDEB_BUILD_GNU_TYPE` \
    --with-libexpat \
    --enable-maintainer-mode \
    CPPFLAGS="-I/usr/local/$HOST/include -D_USE_32BIT_TIME_T" \
    LDFLAGS="-L/usr/local/$HOST/lib" \
    PKG_CONFIG_LIBDIR="/usr/local/$HOST/lib/pkgconfig"
