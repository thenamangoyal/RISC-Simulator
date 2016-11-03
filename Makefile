all: sim

sim: main.o Core.o Memory.o PipelineRegister.o
	g++ main.o Core.o Memory.o PipelineRegister.o -o sim

main.o: main.cpp Core.h Memory.h Register.h PipelineRegister.h
	g++ -c main.cpp

Core.o: Core.cpp Core.h Memory.h Register.h PipelineRegister.h
	g++ -c Core.cpp

Memory.o: Memory.cpp Memory.h
	g++ -c Memory.cpp

PipelineRegister.o: PipelineRegister.cpp PipelineRegister.h Register.h
	g++ -c PipelineRegister.cpp

clean:
	rm *.o sim