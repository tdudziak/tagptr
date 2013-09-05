CXXFLAGS=-DBOOST_TEST_DYN_LINK -std=c++11
LDFLAGS=-lboost_system -lboost_unit_test_framework
SOURCES=main.cpp Allocator.cpp
HEADERS=Allocator.hpp Value.hpp
CXX=clang++

all: test_pool test_heap

test_pool: $(SOURCES) $(HEADERS)
	$(CXX) -DPOOL_ALLOCATOR $(CXXFLAGS) $(LDFLAGS) -o $@ $(SOURCES)

test_heap: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(SOURCES)
