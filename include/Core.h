/*Copyright (C) 2016, IIT Ropar
 * This file is part of SimpleRISC assembler.
 *
 * SimpleRISC assembler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleRISC assembler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Naman Goyal (email: 2015csb1021@iitrpr.ac.in)
 */

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
        
		void reset_proc();
		void load_program_memory(const char* file_name);
		void write_data_memory();
		void write_state();
		void run_simplesim();

		//reads from the instruction memory
		void fetch_begin();
		//updates the instruction register
		void fetch_end();
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
    	bool invalidInputFile;
    	bool pipeline;
    	unsigned int INST_MAX;
    	Memory MEM;	

		PipelineRegister if_of;
		PipelineRegister of_ex;
		PipelineRegister ex_ma;
		PipelineRegister ma_rw;

		Register<unsigned int> PC;
        unsigned int R[16];        
        bool eq, gt;
        bool isBranchTaken;
        unsigned int branchPC;


	private:
		unsigned int mem_address(unsigned int data_address);
		unsigned int inst_bitset(unsigned int inst_word, unsigned int start, unsigned int end);		
		bool check_data_conflict(PipelineRegister& A, PipelineRegister& B);

		bool detect_data_dependency();
		bool detect_control_dependency();
};


#endif // CORE_H
