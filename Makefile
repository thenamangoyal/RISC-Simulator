all: sim

sim:
	g++ -g -I./include src/main.cpp src/Core.cpp src/Memory.cpp src/PipelineRegister.cpp -o ./bin/sim

clean:
	rm -f bin/sim