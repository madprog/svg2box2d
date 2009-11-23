AC_PREREQ(2.50)
AC_INIT([The Escape], [0.1], [paul.morelle@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_FILES([
		 Makefile
		 src/Makefile
		 test/Makefile
		 ])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(LIBRARY_DEPS, box2d >= 2.0.1 libxml-2.0 >= 2.7.6)
PKG_CHECK_MODULES(TEST_DEPS, cairo >= 1.8.8 sdl >= 1.2.13, TEST_DEPS_OK=yes, TEST_DEPS_OK=no)
AM_CONDITIONAL(TEST_DEPS_OK, [test x$TEST_DEPS_OK = xyes])
if test TEST_DEPS_OK != yes; then
	AC_MSG_WARN(["Tests won't be available for the following reason: $TEST_DEPS_PKG_ERRORS"])
fi

AC_OUTPUT
