
CXX=g++
CXXFLAGS= -Wall -march=native --std=c++11 -fopenmp -O3
PROG=bmark_spmv

default: $(PROG)


$(PROG): $(PROG).cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) $(PROGS)
