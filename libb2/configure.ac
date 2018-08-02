AC_PREREQ([2.61])
AC_INIT([libb2], [0.98], [contact@blake2.net], [libb2], [https://blake2.net])
AC_CONFIG_SRCDIR([src/blake2b.c])
AC_CONFIG_HEADERS([src/config.h])
AM_INIT_AUTOMAKE([foreign 1.9])
AC_CONFIG_MACRO_DIR([m4])

B2_LIBRARY_VERSION=1:3:0 # interface, revision, age
AC_SUBST(B2_LIBRARY_VERSION)

AC_LANG_C
AC_PROG_CC
AC_PROG_CC_C99

AC_CHECK_FUNCS(explicit_bzero)
AC_CHECK_FUNCS(explicit_memset)
AC_CHECK_FUNCS(memset_s)
AC_CHECK_HEADERS([stddef.h stdint.h stdlib.h string.h])
AC_OPENMP
# AX_FORCEINLINE()
AC_C_BIGENDIAN(
	[],
	AC_DEFINE(NATIVE_LITTLE_ENDIAN, 1, [machine is little-endian]),
	AC_MSG_ERROR(unknown endianness),
	AC_MSG_ERROR(universal endianness not supported)
) 

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([memset])

dnl AM_PROG_AR 
AC_PROG_LIBTOOL
dnl LT_INIT


AC_ARG_ENABLE(fat,
AC_HELP_STRING([--enable-fat],
               [build a fat binary on systems that support it [default=no]]),
               [case $enableval in
                  yes|no) ;;
                  *) AC_MSG_ERROR([bad value $enableval for --enable-fat, need yes or no]) ;;
                esac],
               [enable_fat=no]
)

AC_ARG_ENABLE(native,
AC_HELP_STRING([--enable-native],
               [build a binary optimized for the CPU found at compile time on systems that support it [default=yes]]),
               [case $enableval in
                  yes|no) ;;
                  *) AC_MSG_ERROR([bad value $enableval for --enable-native, need yes or no]) ;;
                esac],
               [enable_native=yes]
)

AX_CHECK_COMPILE_FLAG([-O3], [CFLAGS=-O3])
dnl Not all architectures support -march=native
if test $enable_native == "yes"; then
  AX_CHECK_COMPILE_FLAG([-march=native], [], [enable_native=no])
fi

if test $enable_fat == "yes"; then
  dnl Fat build needs compiler who knows all the possible instruction sets
  AX_CHECK_COMPILE_FLAG([-msse2],   [], AC_MSG_ERROR([Compiler does not know -msse2.]))
  AX_CHECK_COMPILE_FLAG([-mssse3],  [], AC_MSG_ERROR([Compiler does not know -mssse3.]))
  AX_CHECK_COMPILE_FLAG([-msse4.1], [], AC_MSG_ERROR([Compiler does not know -msse4.1.]))
  AX_CHECK_COMPILE_FLAG([-mavx],    [], AC_MSG_ERROR([Compiler does not know -mavx.]))
  AX_CHECK_COMPILE_FLAG([-mxop],    [], AC_MSG_ERROR([Compiler does not know -mxop.]))
elif test $enable_native == "yes"; then
  AX_EXT
  CFLAGS="${CFLAGS} -march=native ${SIMD_FLAGS}"
fi

case $host_os in
	*mingw*) LDFLAGS="${LDFLAGS} -no-undefined" ;;
esac

AM_CONDITIONAL([USE_FAT], [test "$enable_fat" = "yes"])
dnl Only move away from ref with SSSE3; SSE2 is generally slower
AM_CONDITIONAL([USE_SSE], [test "$ax_cv_have_ssse3_ext" = "yes"])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 ])
AC_OUTPUT
