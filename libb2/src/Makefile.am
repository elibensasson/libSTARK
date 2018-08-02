#   BLAKE2 reference source code package - optimized C implementations
#
#   Written in 2012 by Samuel Neves <sneves@dei.uc.pt>
#
#   To the extent possible under law, the author(s) have dedicated all copyright
#   and related and neighboring rights to this software to the public domain
#   worldwide. This software is distributed without any warranty.
#
#   You should have received a copy of the CC0 Public Domain Dedication along with
#   this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

EXTRA_DIST = 

CPPFLAGS += $(LTDLINCL) $(OPENMP_CFLAGS)
CFLAGS += $(OPENMP_CFLAGS)
LDFLAGS += -version-info $(B2_LIBRARY_VERSION)

lib_LTLIBRARIES = libb2.la
libb2_la_LIBADD = # -lgomp -lpthread
libb2_la_CPPFLAGS =  -DSUFFIX=  \
                     $(LTDLINCL) \
                     ${top_builddir}/src/

include_HEADERS = blake2.h

if USE_FAT
noinst_LTLIBRARIES = libblake2b_ref.la \
                     libblake2b_sse2.la \
                     libblake2b_ssse3.la \
                     libblake2b_sse41.la \
                     libblake2b_avx.la  \
                     libblake2b_xop.la \
                     libblake2s_ref.la \
                     libblake2s_sse2.la \
                     libblake2s_ssse3.la \
                     libblake2s_sse41.la \
                     libblake2s_avx.la  \
                     libblake2s_xop.la 

libb2_la_SOURCES = blake2-dispatch.c blake2sp.c blake2bp.c
libb2_la_LIBADD += libblake2b_ref.la \
                  libblake2b_sse2.la \
				          libblake2b_ssse3.la \
				          libblake2b_sse41.la \
				          libblake2b_avx.la  \
				          libblake2b_xop.la \
				          libblake2s_ref.la \
                  libblake2s_sse2.la \
                  libblake2s_ssse3.la \
                  libblake2s_sse41.la \
                  libblake2s_avx.la  \
                  libblake2s_xop.la 


libblake2b_ref_la_SOURCES = blake2b-ref.c
libblake2b_ref_la_CPPFLAGS = -DSUFFIX=_ref
libblake2b_ref_la_CFLAGS = 

libblake2b_sse2_la_SOURCES = blake2b.c
libblake2b_sse2_la_CPPFLAGS = -DSUFFIX=_sse2 
libblake2b_sse2_la_CFLAGS = -msse2

libblake2b_ssse3_la_SOURCES = blake2b.c
libblake2b_ssse3_la_CPPFLAGS = -DSUFFIX=_ssse3
libblake2b_ssse3_la_CFLAGS = -msse2 -mssse3

libblake2b_sse41_la_SOURCES = blake2b.c
libblake2b_sse41_la_CPPFLAGS = -DSUFFIX=_sse41 
libblake2b_sse41_la_CFLAGS = -msse2 -mssse3 -msse4.1

libblake2b_avx_la_SOURCES = blake2b.c
libblake2b_avx_la_CPPFLAGS = -DSUFFIX=_avx
libblake2b_avx_la_CFLAGS = -msse2 -mssse3 -msse4.1 -mavx

libblake2b_xop_la_SOURCES = blake2b.c
libblake2b_xop_la_CPPFLAGS = -DSUFFIX=_xop 
libblake2b_xop_la_CFLAGS = -msse2 -mssse3 -msse4.1 -mavx -mxop


libblake2s_ref_la_SOURCES = blake2s-ref.c
libblake2s_ref_la_CPPFLAGS = -DSUFFIX=_ref
libblake2s_ref_la_CFLAGS = 

libblake2s_sse2_la_SOURCES = blake2s.c
libblake2s_sse2_la_CPPFLAGS = -DSUFFIX=_sse2
libblake2s_sse2_la_CFLAGS = -msse2

libblake2s_ssse3_la_SOURCES = blake2s.c
libblake2s_ssse3_la_CPPFLAGS = -DSUFFIX=_ssse3
libblake2s_ssse3_la_CFLAGS = -msse2 -mssse3

libblake2s_sse41_la_SOURCES = blake2s.c
libblake2s_sse41_la_CPPFLAGS = -DSUFFIX=_sse41 
libblake2s_sse41_la_CFLAGS = -msse2 -mssse3 -msse4.1

libblake2s_avx_la_SOURCES = blake2s.c
libblake2s_avx_la_CPPFLAGS = -DSUFFIX=_avx
libblake2s_avx_la_CFLAGS = -msse2 -mssse3 -msse4.1 -mavx

libblake2s_xop_la_SOURCES = blake2s.c
libblake2s_xop_la_CPPFLAGS = -DSUFFIX=_xop 
libblake2s_xop_la_CFLAGS = -msse2 -mssse3 -msse4.1 -mavx -mxop

else

if USE_SSE
libb2_la_SOURCES = blake2sp.c \
                   blake2bp.c \
                   blake2s.c \
                   blake2b.c \
                   blake2-impl.h \
                   blake2-config.h \
                   blake2s-round.h \
                   blake2b-round.h \
                   blake2s-load-xop.h \
                   blake2s-load-sse41.h \
                   blake2s-load-sse2.h \
                   blake2b-load-sse41.h \
                   blake2b-load-sse2.h 
else
libb2_la_SOURCES = blake2s-ref.c \
                   blake2b-ref.c \
                   blake2.h \
                   blake2-impl.h \
                   blake2sp.c \
                   blake2bp.c \
                   blake2-kat.h 
endif
endif

TESTS_TARGETS = blake2s-test \
                blake2b-test \
                blake2sp-test \
                blake2bp-test

check_PROGRAMS = $(TESTS_TARGETS)
TESTS = $(TESTS_TARGETS)

TESTS_LDADD = ${top_builddir}/src/libb2.la

blake2s_test_SOURCE = blake2s-test.c blake2-kat.h 
blake2s_test_LDADD = $(TESTS_LDADD)

blake2b_test_SOURCE = blake2b-test.c blake2-kat.h 
blake2b_test_LDADD = $(TESTS_LDADD)

blake2sp_test_SOURCE = blake2sp-test.c blake2-kat.h
blake2sp_test_LDADD = $(TESTS_LDADD)

blake2bp_test_SOURCE = blake2bp-test.c blake2-kat.h
blake2bp_test_LDADD = $(TESTS_LDADD)

