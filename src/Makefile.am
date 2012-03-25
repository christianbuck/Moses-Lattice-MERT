## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" 

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = latticemert \
	bleutest

bleutest_SOURCES = \
	BleuTest.cpp\
	BleuScorer.h \
	Types.h \
	Lattice.cpp \
	Lattice.h \
	Util.h \
	BleuScorer.cpp

latticemert_SOURCES = \
	BleuScorer.cpp      \
	BleuScorer.h      \
	Lattice.cpp      \
	Lattice.h      \
	MosesGraphReader.cpp      \
	MosesGraphReader.h      \
	Parameters.cpp      \
	Parameters.h      \
	Types.h      \
	Util.h      \
	Main.cpp      

latticemert_LDFLAGS = \
	-lboost_regex-mt

