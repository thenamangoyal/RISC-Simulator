#ifndef CORE_H
#define CORE_H
#define MEM_CAPAPCITY 10000

#include <iostream>
#include <ifstream>
#include <sstream>
#include <string>
#include <climits>

#include "Memory.h"


class Core
{
    public:
    	Core();
    	~Core();

        void run_simplesim();
		void reset_proc();
		void load_program_memory(const char* file_name);
		void write_data_memory();

		//reads from the instruction memory and updates the instruction register
		void fetch();
		//reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
		void decode();
		//executes the ALU operation based on ALUop
		void execute();
		//perform the memory operation
		void mem();
		//writes the results back to register file
		void write_back();

    protected:
    
    private:
        unsigned int R[16];
        unsigned int PC;
        bool eq, gt;
        unsigned int instruction_word;
		unsigned int operand1;
		unsigned int operand2;
		Memory MEM(MEM_CAPAPCITY);
		unsigned int INST_MAX;

	private:
		unsigned int mem_address(unsigned int data_address);
};

#endif // CORE_H
