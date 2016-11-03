#ifndef CORE_H
#define CORE_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <bitset>
#include <climits>

#include "Memory.h"
#include "Register.h"
#include "PipelineRegister.h"

const unsigned int MEM_CAPACITY  = 100000;

class Core
{
    public:
    	Core(bool pipe = false);
    	~Core();
        
		void reset_proc();
		void load_program_memory(const char* file_name);
		void write_data_memory();
		void write_state();
		void run_simplesim();

		//reads from the instruction memory
		void fetch();
		//reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
		void decode();
		//executes the ALU operation based on ALUop
		void execute();
		//perform the memory operation
		void mem_access();
		//writes the results back to register file
		void write_back();

    protected:
    
    private:
    	bool pipeline;
    	unsigned int INST_MAX;
    	Memory* MEM;	

		PipelineRegister* if_of;
		PipelineRegister* of_ex;
		PipelineRegister* ex_ma;
		PipelineRegister* ma_rw;

		Regsiter<unsigned int>* PC;
        unsigned int R[16];        
        bool eq, gt;


	private:
		unsigned int mem_address(unsigned int data_address);
		unsigned int inst_bitset(unsigned int inst_word, unsigned int start, unsigned int end);
};


#endif // CORE_H
