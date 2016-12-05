 CXXFLAGS=-Wall -g -I. -pthread

manager: manager.o
	g++ $(CXXFLAGS) -o manager manager.o
manager.o: manager.cpp router.cpp project3.h ospf.cpp
	g++ $(CXXFLAGS) -c manager.cpp
clean:
	-rm -f manager *.o

