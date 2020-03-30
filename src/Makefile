CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

TESTEXENAME = test
EXENAME = skeleton
TESTOBJS = test.o skeleton.o PNG.o pixel.o lodepng.o
OBJS = main.o skeleton.o PNG.o pixel.o lodepng.o

all: $(TESTEXENAME) $(EXENAME)

$(TESTEXENAME): $(TESTOBJS)
	$(CXX) $(CXXFLAGS) $(TESTOBJS) -o $(TESTEXENAME)

$(EXENAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXENAME)

test.o: test.cpp skeleton.h
	$(CXX) $(CXXFLAGS) -c test.cpp

main.o: main.cpp skeleton.h
	$(CXX) $(CXXFLAGS) -c main.cpp

skeleton.o: skeleton.cpp PNG.h
	$(CXX) $(CXXFLAGS) -c skeleton.cpp

PNG.o: PNG.cpp PNG.h pixel.h lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) -c PNG.cpp

pixel.o: pixel.cpp pixel.h
	$(CXX) $(CXXFLAGS) -c pixel.cpp

lodepng.o: lodepng/lodepng.cpp lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) -c lodepng/lodepng.cpp

clean:
	rm -rf *.o skeleton test ../out/*
