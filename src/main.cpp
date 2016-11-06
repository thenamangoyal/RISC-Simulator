#include <iostream>
#include <ctime>

#include "Core.h"
#include "Memory.h"
#include "Register.h"
#include "PipelineRegister.h"

using namespace std;


int main(int argc, char *argv[])
{
	clock_t begin = clock();

	if (argc<2){
		cout<<"Please provide Instruction MEM file as an argument"<<endl;
		return 1;
	}

	bool to_pipeline = false;

	if (argc >= 3){
		if (argv[2][0] == '1'){
			to_pipeline = true;
		}

	}

	Core simulator(to_pipeline);

	simulator.load_program_memory(argv[1]);

	simulator.reset_proc();

	simulator.run_simplesim();

	simulator.write_data_memory();

	simulator.write_state();

	clock_t end = clock();
	double time = double(end - begin) / CLOCKS_PER_SEC;

	cout<<endl<<"Running time: "<<dec<<time<<endl;

    return 0;
}
