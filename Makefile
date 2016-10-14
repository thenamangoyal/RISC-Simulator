all: sim

sim: main.o Core.o Memory.o
	g++ main.o Core.o Memory.o -o sim

main.o: main.cpp Core.h Memory.h
	g++ -c main.cpp

core.o: core.cpp Core.h Memory.h
	g++ -c core.cpp

Memory.o: Memory.cpp Memory.h
	g++ -c Memory.cpp

clean:
	rm *.o sim