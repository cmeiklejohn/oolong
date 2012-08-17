bin_PROGRAMS=oolong

oolong_SOURCES=main.c
oolong_LDFLAGS=-lncurses
oolong_LDADD=liboolong.la

lib_LTLIBRARIES = liboolong.la
liboolong_la_SOURCES=oolong.c
liboolong_la_LDFLAGS=-version-info 1:1:1 -lmsgpack
