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

	simulator.fetch();
	simulator.decode();


	simulator.write_data_memory();


    /*Memory mem_Block(12);
    mem_Block.Write(0,5);
    mem_Block.Write(4,-10);
    cout<<(signed int)mem_Block.Read(4)<<endl;*/
    return 0;
}
