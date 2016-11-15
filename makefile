 CXXFLAGS=-Wall -g -I. -pthread

manager: manager.o router.o
	g++ $(CXXFLAGS) -o manager manager.o
manager.o: manager.cpp router.cpp project3.h
	g++ $(CXXFLAGS) -c manager.cpp router.cpp
clean:
	-rm -f manager *.o

