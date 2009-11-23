if TEST_DEPS_OK
bin_PROGRAMS=svg2box2d_test

svg2box2d_test_SOURCES=\
		       Game.cpp \
		       SvgDrawer.cpp \
		       main.cpp
svg2box2d_test_CXXFLAGS=\
			$(LIBRARY_DEPS_CFLAGS) \
			$(TEST_DEPS_CFLAGS)
svg2box2d_test_LDADD=\
			$(LIBRARY_DEPS_LIBS) \
			$(TEST_DEPS_LIBS) \
			-L../src/.libs -lsvg2box2d
endif
