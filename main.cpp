#include <iostream>

#include "Core.h"
#include "Memory.h"

using namespace std;


int main(int argc, char *argv[])
{
	if (argc<0){
		cout<<"Please provide Instruction MEM file as an argument"<<endl;
		return 1;
	}

	Core simulator;

	simulator.load_program_memory(argv[1]);

	simulator.reset_proc();



    Memory mem_Block(12);
    mem_Block.Write(0,5);
    mem_Block.Write(4,-10);
    cout<<(signed int)mem_Block.Read(4)<<endl;
    return 0;
}
