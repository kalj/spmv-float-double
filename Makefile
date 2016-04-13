
CXX=g++
CXXFLAGS= -Wall -march=native --std=c++11 -fopenmp -O3
BMPROG=bmark_spmv
TESTPROG=test_spmv

default: $(BMPROG)


$(BMPROG): $(BMPROG).cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TESTPROG): $(TESTPROG).cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) $(BMPROG) $(TESTPROG)
