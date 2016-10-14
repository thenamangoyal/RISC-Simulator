#include <iostream>

#include "Core.h"
#include "Memory.h"

using namespace std;


int main(int argc, char *argv[])
{
	if (argc<2){
		cout<<"Please provide Instruction MEM file as an argument"<<endl;
		return 1;
	}

	Core simulator;

	simulator.load_program_memory(argv[1]);

	simulator.reset_proc();

	simulator.run_simplesim();

	simulator.write_data_memory();

    return 0;
}
