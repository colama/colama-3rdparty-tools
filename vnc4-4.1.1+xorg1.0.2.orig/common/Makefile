# Generated automatically from Makefile.in:boilerplate.mk by configure.

SUBDIRS = zlib rdr network Xregion rfb

DIR_CFLAGS = -DPIC -fPIC
DIR_CXXFLAGS = -DPIC -fPIC
# followed by boilerplate.mk

all::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done

clean::
	@subdirs="$(SUBDIRS)"; for d in $$subdirs; do (cd $$d; $(MAKE) $@) || exit 1; done

clean::
	rm -f $(program) $(library) *.o

SHELL = /bin/sh
top_srcdir = .

CC = gcc
CFLAGS = -O2 -Wall $(DIR_CFLAGS)
CCLD = $(CC)
CXX = g++
CXXFLAGS = -O2 -Wall $(DIR_CXXFLAGS)
CXXLD = $(CXX)
CPPFLAGS = 
DEFS =  -DX_DISPLAY_MISSING=1 
ALL_CPPFLAGS = $(CPPFLAGS) $(DEFS) $(DIR_CPPFLAGS)
LIBS = 
LDFLAGS = 
RANLIB = ranlib
AR = ar cq

.SUFFIXES:
.SUFFIXES: .cxx .c .o

.c.o:
	$(CC) $(ALL_CPPFLAGS) $(CFLAGS) -c $<

.cxx.o:
	$(CXX) $(ALL_CPPFLAGS) $(CXXFLAGS) -c $<
