#include "Core.h"

using namespace std;


Core::Core(){
	INST_MAX = 0;
	MEM = new Memory(MEM_CAPACITY);
}

Core::~Core(){
	delete MEM;
}

void Core::load_program_memory(const char* file_name){
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
			line>>hex>>address;
			line>>hex>>inst;
			MEM->Write(address, inst);
			INST_MAX += 4;
		}		
	}
	inst_file.close();
}

void Core::write_data_memory() {
	ofstream out_file;
	out_file.open("DATA_OUT.mem",ios::out | ios::trunc);

  	for(int i=0; i < MEM_CAPACITY; i+= 4){
	   out_file<<"0x"<<hex<<i<<" "<<"0x"<<hex<<MEM->Read(i)<<endl;
	}
	out_file.close();
}


void Core::reset_proc()
{
	for(int i=0;i<16;i++)
	{
		R[i] = 0;
	}
	R[14] = MEM_CAPACITY - INST_MAX; // Stack Pointer initialize
	PC = 0;
	eq = gt = false;
	instruction_word = 0;
	operand1 = 0;
	operand2 = 0; 
}

//reads from the instruction memory and updates the instruction register
void Core::fetch() {
	instruction_word = MEM->Read(PC);
	cout<<"Reading instruction 0x"<<hex<<instruction_word<<" at address 0x"<<hex<<PC<<endl;
		
}
//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void Core::decode() {
	unsigned int opcode1 = inst_bitset(instruction_word, 28,28);
	unsigned int opcode2 = inst_bitset(instruction_word, 29,29);
	unsigned int opcode3 = inst_bitset(instruction_word, 30,30);
	unsigned int opcode4 = inst_bitset(instruction_word, 31,31);
	unsigned int opcode5 = inst_bitset(instruction_word, 32,32);

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1 && opcode1 == 1){
		isSt = true;
		cout<<"Is Store"<<endl;
	}
	else{
		isSt = false;
	}
	


}
//executes the ALU operation based on ALUop
void Core::execute() {
}
//perform the memory operation
void Core::mem_access() {
}
//writes the results back to register file
void Core::write_back() {
}

unsigned int Core::mem_address(unsigned int data_address){
	return data_address + INST_MAX;
}

unsigned int Core::inst_bitset(unsigned int inst_word, unsigned int start, unsigned int end){
	inst_word = inst_word<<(32-end);
	inst_word = inst_word>>(32-end + start-1);
	return inst_word;
}

