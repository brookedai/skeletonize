CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

EXENAME = skeleton
OBJS = skeleton.o PNG.o pixel.o lodepng.o

$(EXENAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXENAME)

skeleton.o: skeleton.cpp PNG.h
	$(CXX) $(CXXFLAGS) -c skeleton.cpp

PNG.o: PNG.cpp PNG.h pixel.h lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) -c PNG.cpp

pixel.o: pixel.cpp pixel.h
	$(CXX) $(CXXFLAGS) -c pixel.cpp

lodepng.o: lodepng/lodepng.cpp lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) -c lodepng/lodepng.cpp

clean:
	rm -rf *.o skeleton ../out/*
