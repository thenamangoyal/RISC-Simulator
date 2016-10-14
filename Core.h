#ifndef CORE_H
#define CORE_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <bitset>
#include <climits>

#include "Memory.h"

const unsigned int MEM_CAPACITY  = 100;

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
		void mem_access();
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
		Memory* MEM;
		unsigned int INST_MAX;

		bool isSt;
		bool isLd;
		bool isBeq;
		bool isBgt;
		bool isRet;
		bool isImmediate;
		bool isWb;
		bool isUbranch;
		bool isCall;
		bool isAdd;
		bool isSub;
		bool isCmp;
		bool isMul;
		bool isDiv;
		bool isMod;
		bool isLsl;
		bool isLsr;
		bool isAsr;
		bool inOr;
		bool isAnd;
		bool isNot;
		bool isMov;



	private:
		unsigned int mem_address(unsigned int data_address);
		unsigned int inst_bitset(unsigned int inst_word, unsigned int start, unsigned int end);
};


#endif // CORE_H
