CXX ?= cc

OBJS=MosesGraphReader.o Lattice.o BleuScorer.o Main.o Parameters.o

CXXFLAGS = -Wall -Wfatal-errors -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES 
LDFLAGS =  -lstdc++ -lboost_regex-mt

GPROF2DOT = ./gprof2dot.py -n 5.0 -e 2.0

ifeq ($(DEBUG_OPTION),yes)
CXXFLAGS += -g
LDFLAGS += -g
else
OPTIMIZE_OPTION ?= yes
endif

ifeq ($(PROFILE_OPTION),yes)
CXXFLAGS += -pg -lc -g
LDFLAGS += -pg -lc -g
OPTIMIZE_OPTION = no
endif

ifeq ($(OPTION),iccprof)
CXXFLAGS += -debug full
endif


ifeq ($(OPTIMIZE_OPTION),yes)
CXXFLAGS += -O3
else
CXXFLAGS += -O0
endif

all: LatticeMERT BleuTest

BleuTest: BleuTest.o BleuScorer.o Makefile
	@echo "***>" BleuTest "<***"
	$(CXX) $(CXXFLAGS) BleuTest.o BleuScorer.o $(LDFLAGS) -o BleuTest

LatticeMERT: $(OBJS) Makefile
	@echo "***>" LatticeMERT "<***"
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o LatticeMERT  

%.o : %.cpp Types.h
	@echo "***" $< "***"
	$(CXX) $(CXXFLAGS) -c $< -o $@  

.PHONY : all clean prof debug graph
clean:
	rm -f *.o *~ LatticeMERT

debug: Makefile
	$(MAKE) $(MAKEFILE) DEBUG_OPTION=yes 

prof: Makefile
	$(MAKE) $(MAKEFILE) PROFILE_OPTION=yes

#gmon.out: prof
#	time ./LatticeMERT > log.txt

graph: gmon.out
	gprof ./LatticeMERT | $(GPROF2DOT) -s | dot -Tpdf -o callgraph.pdf

