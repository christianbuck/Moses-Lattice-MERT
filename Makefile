CXX = cc

OBJS=MosesGraphReader.o Lattice.o BleuScorer.o Main.o

CXXFLAGS = -Wall -Wfatal-errors -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES
LDFLAGS =  -lstdc++ -lboost_regex 

ifeq ($(DEBUG_OPTION),yes)
CXXFLAGS += -g
LDFLAGS += -g
else
OPTIMIZE_OPTION = yes
endif

ifeq ($(PROFILE_OPTION),yes)
CXXFLAGS += -pg
LDFLAGS += -pg
endif

ifeq ($(OPTIMIZE_OPTION),yes)
CXXFLAGS += -O3
endif

LatticeMERT: $(OBJS) Makefile
	@$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o LatticeMERT  

%.o : %.cpp
	@echo "***" $< "***"
	@$(CXX) $(CXXFLAGS) -c $< -o $@  

clean:
	rm -f *.o *~ LatticeMERT

debug: Makefile clean
	$(MAKE) $(MAKEFILE) DEBUG_OPTION=yes 

prof: Makefile clean
	$(MAKE) $(MAKEFILE) PROFILE_OPTION=yes

again: clean LatticeMERT
