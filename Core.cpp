#include "Core.h"

using namespace std;


Core::Core(){
	INST_MAX = 0;
}

Core::~Core(){	
}

void load_program_memory(const char* file_name){
	ifstream inst_file;
	inst_file.open(file_name,ios::in);
	string temp;
	while(getline(inst_file, temp)){
		if (temp[0] == '#'){
			// Input comments
		}
		else {
			stringstream line(temp);
			unsigned int address;
			unsigned int inst;
			line>>address;
			line>>inst;
			MEM.Write(address, inst);
			INST_MAX++;
		}		
	}
}

void Core::reset_proc()
{
	for(int i=0;i<16;i++)
	{
		R[i] = 0;
	}
	R[14] = MEM_CAPACITY - INST_MAX - 1; // Stack Pointer given
	PC = 0;
	eq = gt = False;
	instruction_word = 0;
	operand1 = 0;
	operand2 = 0; 
}

unsigned int Core::mem_address(unsigned int data_address){
	return data_address + INST_MAX;
}