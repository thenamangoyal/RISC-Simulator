/*Copyright (C) 2016, IIT Ropar
 * This file is part of SimpleRISC simulator.
 *
 * SimpleRISC simulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SimpleRISC simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Naman Goyal (email: 2015csb1021@iitrpr.ac.in)
 */

#include "Core.h"

#define pprint(x) if (pipeline && (debugLevel >= x)) output_file
#define fprint(x) if (!pipeline && (debugLevel >= x)) output_file

using namespace std;

Core::Core(): MEM(MEM_CAPACITY), if_of(pipeline), of_ex(pipeline), ex_ma(pipeline), ma_rw(pipeline), PC(true) {
	
	for (unsigned int i= 0; i <= MEM_CAPACITY - sizeof(unsigned int); i+= 4){
		MEM.Write(i, 0xffffffff);
	}
	if ((MEM_CAPACITY%4) != 0){
		MEM.Write(MEM_CAPACITY - sizeof(unsigned int), 0xffffffff);
	}

	input_file.open(inputFileName , ios::in);
	output_file.open(outputFileName, ios::out | ios::trunc);

}


void Core::load_program_memory(){

	string temp;
	while(getline(input_file, temp)){
		if (temp[0] == '#'){
			// Input comments
		}
		else {
			stringstream line(temp);
			unsigned int address;
			unsigned int inst;
			bool isReadAddr = (line>>hex>>address);
			bool isReadInst = (line>>hex>>inst);

			if (isReadAddr && isReadInst) {
				if (address >= 0 && address <= MEM_CAPACITY - sizeof(unsigned int)){
					MEM.Write(address, inst);
				}				
			}
			else {
				// Invalid Encoding in Input MEM file
			}

			
		}
	}

	input_file.close();
	
}

void Core::write_memory() {
	if (specifymemout){
		ofstream data_file;
		data_file.open(memoutFileName,ios::out | ios::trunc);

	  	for(int i=0; i <= MEM_CAPACITY - sizeof(unsigned int); i+= 4){
			data_file<<"0x"<<hex<<i<<" "<<"0x"<<hex<<MEM.Read(i)<<endl;
		}
		if ((MEM_CAPACITY%4) != 0){
			data_file<<"*0x"<<hex<<(MEM_CAPACITY - sizeof(unsigned int))<<" "<<"0x"<<hex<<MEM.Read(MEM_CAPACITY - sizeof(unsigned int))<<endl;
		}
		data_file.close();
	}
}

void Core::write_context() {
	if (specifycontextout){
		ofstream context_file;
		context_file.open(contextoutFileName,ios::out | ios::trunc);

		context_file<<"Registers"<<endl<<endl;

		for(int i=0; i<16; i++){
			context_file<<"R"<<dec<<i<<" : "<<dec<<R[i]<<endl;
		}

		context_file<<endl;
		context_file<<"PC : 0x"<<hex<<PC.Read()<<endl;
		context_file<<endl;

		if (eq){
			context_file<<"Flags.eq : True"<<endl;
		}
		else {
			context_file<<"Flags.eq : False"<<endl;
		}

		if (gt){
			context_file<<"Flags.gt : True"<<endl;
		}
		else {
			context_file<<"Flags.gt : False"<<endl;
		}
	  	
		context_file.close();
	}
}


void Core::reset_proc()
{
	for(int i=0;i<16;i++)
	{
		R[i] = 0;
	}
	R[14] = MEM_CAPACITY; // Stack Pointer initialize
	PC.Write(0);
	PC.clock();

	if_of.WriteBubble(pipeline);
	of_ex.WriteBubble(pipeline);
	ex_ma.WriteBubble(pipeline);
	ma_rw.WriteBubble(pipeline);

	if_of.clock();
	of_ex.clock();
	ex_ma.clock();
	ma_rw.clock();

	eq = false;
	gt = false;
	isBranchTaken = false;
}


void Core::run_simplesim(){

	bool isDataDependency;
	bool isControlDependency;

	pprint(1)<<"+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+"<<endl;
	pprint(1)<<"| CYCLE  | FETCH  | DECODE | EXECUT | MEMORY | WRITE  |   R0   |   R1   |   R2   |   R3   |   R4   |   R5   |   R6   |   R7   |   R8   |   R9   |  R10   |  R11   |  R12   |  R13   |   SP   |   RA   |"<<endl;
	pprint(1)<<"+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+--------+"<<endl;

	int counter = 0;
	while ( checkValidPC(PC.Read()) || ( pipeline && ((if_of.bubble.Read() == false) || (of_ex.bubble.Read() == false) || (ex_ma.bubble.Read() == false) || (ma_rw.bubble.Read() == false)) )){
		//pprint(2)<<"========================"<<endl;
		//pprint(2)<<"CYCLE "<<dec<<counter+1<<endl;
		//pprint(2)<<"========================"<<endl;

		fprint(1)<<dec<<counter+1<<":";
		pprint(1)<<"|"<<right<<dec<<setw(7)<<counter+1<<" |";

		if (checkValidPC(PC.Read())) {
			pprint(1)<<" I"<<left<<dec<<setw(6)<<((PC.Read())/4 + 1)<<"|";
		}
		else {
			pprint(1)<<"  ....  |";
		}

		if (if_of.bubble.Read()){
			pprint(1)<<"  ....  |";
		}
		else {
			pprint(1)<<" I"<<left<<dec<<setw(6)<<((if_of.PC.Read())/4 + 1)<<"|";
		}

		if (of_ex.bubble.Read()){
			pprint(1)<<"  ....  |";
		}
		else {
			pprint(1)<<" I"<<left<<dec<<setw(6)<<((of_ex.PC.Read())/4 + 1)<<"|";
		}
		
		if (ex_ma.bubble.Read()){
			pprint(1)<<"  ....  |";
		}
		else {
			pprint(1)<<" I"<<left<<dec<<setw(6)<<((ex_ma.PC.Read())/4 + 1)<<"|";
		}

		if (ma_rw.bubble.Read()){
			pprint(1)<<"  ....  |";
		}
		else {
			pprint(1)<<" I"<<left<<dec<<setw(6)<<((ma_rw.PC.Read())/4 + 1)<<"|";
		}

		for (int k = 0; k< 16 ; k++){
			pprint(1)<<right<<hex<<setw(8)<<R[k]<<"|";
		}

		fetch_begin();
		decode();
		execute();
		mem_access();
		write_back();
		fetch_end();



		isDataDependency = detect_data_dependency();
		isControlDependency = detect_control_dependency();

		if (!isDataDependency && !isControlDependency){

			PC.clock();
			if_of.clock();
			of_ex.clock();
			ex_ma.clock();
			ma_rw.clock();

		}
		else if (isDataDependency && !isControlDependency){
			//pprint(2)<<endl;
			//pprint(2)<<"+-----------------------+"<<endl;
			//pprint(2)<<"|    Data Dependency    |"<<endl;
			//pprint(2)<<"| Stalling PC and IF-OF |"<<endl;
			//pprint(2)<<"|     Bubbling OF-EX    |"<<endl;
			//pprint(2)<<"+-----------------------+"<<endl;
			//pprint(2)<<endl;

			of_ex.WriteBubble(true);
			of_ex.clock();

			ex_ma.clock();
			ma_rw.clock();

		}
		else{
			//pprint(2)<<endl;
			//pprint(2)<<"+-----------------------+"<<endl;
			//pprint(2)<<"|   Control Dependency  |"<<endl;
			//pprint(2)<<"|     Bubbling IF-OF    |"<<endl;
			//pprint(2)<<"|     Bubbling OF-EX    |"<<endl;
			//pprint(2)<<"+-----------------------+"<<endl;
			//pprint(2)<<endl;

			PC.clock();

			if_of.WriteBubble(true);
			if_of.clock();

			of_ex.WriteBubble(true);
			of_ex.clock();

			ex_ma.clock();
			ma_rw.clock();
		}

		//pprint(2)<<"New PC = 0x"<<hex<<PC.Read()<<endl;
		//pprint(2)<<endl;

		fprint(2)<<endl;
		pprint(2)<<endl;

		counter++;
	}
	
	//pprint(2)<<endl;
	//pprint(2)<<"+----------------------------------+"<<endl;
	//pprint(2)<<"Total number of cycles  are "<<dec<<counter<<endl;
	//pprint(2)<<"+----------------------------------+"<<endl;

	output_file.close();

}

//reads from the instruction memory
void Core::fetch_begin() {
	//pprint(2)<<endl<<"!--------- FETCH ---------!"<<endl<<endl;

	unsigned int temp_PC = PC.Read();
	if (checkValidPC(PC.Read())){
		unsigned int temp_instruction_word = MEM.Read(temp_PC);	

		//pprint(2)<<"Instruction 0x"<<hex<<temp_instruction_word<<" read at address 0x"<<hex<<temp_PC<<endl;
		fprint(1)<<" PC=0x"<<hex<<temp_PC<<", 0x"<<hex<<temp_instruction_word;
		if_of.instruction_word.Write(temp_instruction_word);
		if_of.WriteBubble(false);
		if_of.PC.Write(temp_PC);

	}
	else {
		//pprint(2)<<"Invalid PC passing Bubble Instruction"<<endl;
		if_of.WriteBubble(true);
	}
	
}


//updates the instruction register
void Core::fetch_end() {

	unsigned int temp_PC = PC.Read();
	
	if (isBranchTaken){
		PC.Write(branchPC);
	}
	else if (checkValidPC(PC.Read())) {
		PC.Write(temp_PC + 4);
	}

}

//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void Core::decode() {
	//pprint(2)<<endl<<"!--------- DECODE ---------!"<<endl<<endl;

	bool bubble_inst = if_of.bubble.Read();

	if (bubble_inst){
		//pprint(2)<<"<<<< Pipeline Bubble >>>>"<<endl<<endl;
	}
	
	unsigned int temp_PC = if_of.PC.Read();
	unsigned int temp_instruction_word = if_of.instruction_word.Read();

	//pprint(2)<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

	bool temp_isSt;
	bool temp_isLd;
	bool temp_isBeq;
	bool temp_isBgt;
	bool temp_isRet;
	bool temp_isImmediate;
	bool temp_isWb;
	bool temp_isUbranch;
	bool temp_isCall;
	bool temp_isAdd;
	bool temp_isSub;
	bool temp_isCmp;
	bool temp_isMul;
	bool temp_isDiv;
	bool temp_isMod;
	bool temp_isLsl;
	bool temp_isLsr;
	bool temp_isAsr;
	bool temp_isOr;
	bool temp_isAnd;
	bool temp_isNot;
	bool temp_isMov;

	
	unsigned int opcode1 = inst_bitset(temp_instruction_word, 28, 28);
	unsigned int opcode2 = inst_bitset(temp_instruction_word, 29, 29);
	unsigned int opcode3 = inst_bitset(temp_instruction_word, 30, 30);
	unsigned int opcode4 = inst_bitset(temp_instruction_word, 31, 31);
	unsigned int opcode5 = inst_bitset(temp_instruction_word, 32, 32);
	unsigned int I_bit = inst_bitset(temp_instruction_word, 27, 27);
	//pprint(2)<<"Control Signals"<<endl;
	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1 && opcode1 == 1){
		temp_isSt = true;
		//pprint(2)<<"isSt ";
	}
	else{
		temp_isSt = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1 && opcode1 == 0){
		temp_isLd = true;
		//pprint(2)<<"isLd ";
	}
	else{
		temp_isLd = false;
	}
	
	if(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 0){
		temp_isBeq = true;
		//pprint(2)<<"isBeq ";
	}
	else{
		temp_isBeq = false;
	}

	if(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 1){
		temp_isBgt = true;
		//pprint(2)<<"isBgt ";
	}
	else{
		temp_isBgt = false;
	}

	if(opcode5 == 1 && opcode4 == 0 && opcode3 == 1 && opcode2 == 0 && opcode1 == 0){
		temp_isRet = true;
		//pprint(2)<<"isRet ";
	}
	else{
		temp_isRet = false;
	}

	if(I_bit == 1){
		temp_isImmediate = true;
		//pprint(2)<<"isImmediate ";
	}
	else{
		temp_isImmediate = false;
	}

	if( !(opcode5 == 1 || ( opcode5 == 0 && opcode3 == 1 && opcode1 == 1 && ( opcode4 == 1 || opcode2 == 0) ) ) 	|| 	(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1)  ){
		temp_isWb = true;
		//pprint(2)<<"isWb ";
	}
	else{
		temp_isWb = false;
	}

	if( opcode5 == 1 && opcode4 == 0 && (	(opcode3 == 0 && opcode2 == 1) || (opcode3 == 1 && opcode2 == 0 && opcode1 == 0) ) 	){
		temp_isUbranch = true;
		//pprint(2)<<"isUbranch ";
	}
	else{
		temp_isUbranch = false;
	}

	if(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1){
		temp_isCall = true;
		//pprint(2)<<"isCall ";
	}
	else{
		temp_isCall = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 0){
		temp_isAdd = true;
		//pprint(2)<<"isAdd ";
	}
	else if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1){
		temp_isAdd = true;
		//pprint(2)<<"isAdd ";
	}
	else{
		temp_isAdd = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 1){
		temp_isSub = true;
		//pprint(2)<<"isSub ";
	}
	else{
		temp_isSub = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 0 && opcode1 == 1){
		temp_isCmp = true;
		//pprint(2)<<"isCmp ";
	}
	else{
		temp_isCmp = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 0){
		temp_isMul = true;
		//pprint(2)<<"isMul ";
	}
	else{
		temp_isMul = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1){
		temp_isDiv = true;
		//pprint(2)<<"isDiv ";
	}
	else{
		temp_isDiv = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 0 && opcode1 == 0){
		temp_isMod = true;
		//pprint(2)<<"isMod ";
	}
	else{
		temp_isMod = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 1 && opcode1 == 0){
		temp_isLsl = true;
		//pprint(2)<<"isLsl ";
	}
	else{
		temp_isLsl = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1){
		temp_isLsr = true;
		//pprint(2)<<"islsr ";
	}
	else{
		temp_isLsr = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 0 && opcode1 == 0){
		temp_isAsr = true;
		//pprint(2)<<"isAsr ";
	}
	else{
		temp_isAsr = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 1 && opcode1 == 1){
		temp_isOr = true;
		//pprint(2)<<"isOr ";
	}
	else{
		temp_isOr = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 1 && opcode1 == 0){
		temp_isAnd = true;
		//pprint(2)<<"isAnd ";
	}
	else{
		temp_isAnd = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 0 && opcode1 == 0){
		temp_isNot = true;
		//pprint(2)<<"isNot ";
	}
	else{
		temp_isNot = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 0 && opcode1 == 1){
		temp_isMov = true;
		//pprint(2)<<"isMov ";
	}
	else{
		temp_isMov = false;
	}
	//pprint(2)<<endl;


	//////////   immx calculation  ///////////
	//pprint(2)<<endl;
	unsigned int temp_imm = inst_bitset(temp_instruction_word, 1, 16);
	unsigned int temp_u = inst_bitset(temp_instruction_word, 17, 17);
	unsigned int temp_h = inst_bitset(temp_instruction_word, 18, 18);
	unsigned int temp_immx;


	if (temp_u == 0 && temp_h == 0){
		if (inst_bitset(temp_instruction_word, 16, 16) == 1){
			temp_immx = 0xffff0000 | temp_imm;
		}
		else{
			temp_immx = temp_imm;
		}
		//pprint(2)<<"Immediate is "<<dec<<temp_immx<<" (0x"<<hex<<temp_immx<<")"<<endl;
	}
	else if (temp_u == 1){
		temp_immx = temp_imm;
		//pprint(2)<<"Immediate is "<<dec<<temp_immx<<" (0x"<<hex<<temp_immx<<") and is Unsigned"<<endl;
	}
	else{
		temp_immx = temp_imm<<16;
		//pprint(2)<<"Immediate is "<<dec<<temp_immx<<" (0x"<<hex<<temp_immx<<") ans is High"<<endl;
	}


	//////////   branchTarget calculation  ///////////
	unsigned int temp_branchTarget;
	unsigned int temp_offset = inst_bitset(temp_instruction_word, 1,27);

	if (inst_bitset(temp_instruction_word, 27, 27) == 1){
		temp_branchTarget = 0xe0000000 | (temp_offset<<2);
	}
	else {
		temp_branchTarget = temp_offset<<2;
	}
	
	temp_branchTarget += temp_PC;
	//pprint(2)<<endl<<"branchTarget is 0x"<<hex<<temp_branchTarget<<endl;


	//////////   Reading Register File  ///////////
	unsigned int temp_operand1;
	unsigned int temp_operand2;
	unsigned int temp_rd = inst_bitset(temp_instruction_word, 23,26);
	unsigned int temp_rs1 = inst_bitset(temp_instruction_word, 19,22);
	unsigned int temp_rs2 = inst_bitset(temp_instruction_word, 15,18);

	//pprint(2)<<endl<<"rd: R"<<dec<<temp_rd<<", rs1: R"<<dec<<temp_rs1<<", rs2: R"<<dec<<temp_rs2<<endl;

	if (temp_isRet){
		temp_operand1 = R[15];
		//pprint(2)<<"Operand1: "<<dec<<temp_operand1<<" (Read from ra OR R15)"<<endl;
		fprint(1)<<", R15 = ";
	}
	else{
		temp_operand1 = R[temp_rs1];
		//pprint(2)<<"Operand1: "<<dec<<temp_operand1<<" (Read from rs1)"<<endl;
		fprint(1)<<", R"<<dec<<temp_rs1<<" = ";
	}
	fprint(1)<<dec<<temp_operand1;
	

	if (temp_isSt){
		temp_operand2 = R[temp_rd];
		//pprint(2)<<"Operand2: "<<dec<<temp_operand2<<" (Read from rd)"<<endl;
		if (!temp_isImmediate){
			fprint(1)<<", R"<<dec<<temp_rd<<" = ";
		}		
	}
	else{
		temp_operand2 = R[temp_rs2];
		//pprint(2)<<"Operand2: "<<dec<<temp_operand2<<" (Read from rs2)"<<endl;
		if (!temp_isImmediate){
			fprint(1)<<", R"<<dec<<temp_rs2<<" = ";
		}
	}
	if (!temp_isImmediate){
		fprint(1)<<dec<<temp_operand2;
	}
	else{
		fprint(1)<<", Imm = "<<dec<<temp_immx;
	}	
	

	unsigned int temp_A = temp_operand1;
	//pprint(2)<<"A: "<<dec<<temp_A<<" (operand1)"<<endl;

	unsigned int temp_B;

	if (temp_isImmediate){
		temp_B = temp_immx;
		//pprint(2)<<"B: "<<dec<<temp_B<<" (immx)"<<endl;
		
	}
	else {
		temp_B = temp_operand2;
		//pprint(2)<<"B: "<<dec<<temp_B<<" (operand2)"<<endl;
	}

	of_ex.PC.Write(temp_PC);
	of_ex.instruction_word.Write(temp_instruction_word);

	of_ex.branchTarget.Write(temp_branchTarget);

	of_ex.A.Write(temp_A);
	of_ex.B.Write(temp_B);
	of_ex.operand2.Write(temp_operand2);

	of_ex.isSt.Write(temp_isSt);
	of_ex.isLd.Write(temp_isLd);
	of_ex.isBeq.Write(temp_isBeq);
	of_ex.isBgt.Write(temp_isBgt);
	of_ex.isRet.Write(temp_isRet);
	of_ex.isImmediate.Write(temp_isImmediate);
	of_ex.isWb.Write(temp_isWb);
	of_ex.isUbranch.Write(temp_isUbranch);
	of_ex.isCall.Write(temp_isCall);
	of_ex.isAdd.Write(temp_isAdd);
	of_ex.isSub.Write(temp_isSub);
	of_ex.isCmp.Write(temp_isCmp);
	of_ex.isMul.Write(temp_isMul);
	of_ex.isDiv.Write(temp_isDiv);
	of_ex.isMod.Write(temp_isMod);
	of_ex.isLsl.Write(temp_isLsl);
	of_ex.isLsr.Write(temp_isLsr);
	of_ex.isAsr.Write(temp_isAsr);
	of_ex.isOr.Write(temp_isOr);
	of_ex.isAnd.Write(temp_isAnd);
	of_ex.isNot.Write(temp_isNot);
	of_ex.isMov.Write(temp_isMov);

	of_ex.ForwardBubble(bubble_inst);

}

//executes the ALU operation based on ALUop
void Core::execute() {
	//pprint(2)<<endl<<"!--------- EXECUTE ---------!"<<endl<<endl;	

	bool bubble_inst = of_ex.bubble.Read();

	if (bubble_inst){
		//pprint(2)<<"<<<< Pipeline Bubble >>>>"<<endl<<endl;
	}
	
	unsigned int temp_PC = of_ex.PC.Read();
	unsigned int temp_instruction_word = of_ex.instruction_word.Read();

	//pprint(2)<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

	unsigned int temp_branchTarget = of_ex.branchTarget.Read();

	unsigned int temp_A = of_ex.A.Read();	
	unsigned int temp_B = of_ex.B.Read();
	unsigned int temp_operand2 = of_ex.operand2.Read();

	bool temp_isSt = of_ex.isSt.Read();
	bool temp_isLd = of_ex.isLd.Read();
	bool temp_isBeq = of_ex.isBeq.Read();
	bool temp_isBgt = of_ex.isBgt.Read();
	bool temp_isRet = of_ex.isRet.Read();
	bool temp_isImmediate = of_ex.isImmediate.Read();
	bool temp_isWb = of_ex.isWb.Read();
	bool temp_isUbranch = of_ex.isUbranch.Read();
	bool temp_isCall = of_ex.isCall.Read();
	bool temp_isAdd = of_ex.isAdd.Read();
	bool temp_isSub = of_ex.isSub.Read();
	bool temp_isCmp = of_ex.isCmp.Read();
	bool temp_isMul = of_ex.isMul.Read();
	bool temp_isDiv = of_ex.isDiv.Read();
	bool temp_isMod = of_ex.isMod.Read();
	bool temp_isLsl = of_ex.isLsl.Read();
	bool temp_isLsr = of_ex.isLsr.Read();
	bool temp_isAsr = of_ex.isAsr.Read();
	bool temp_isOr = of_ex.isOr.Read();
	bool temp_isAnd = of_ex.isAnd.Read();
	bool temp_isNot = of_ex.isNot.Read();
	bool temp_isMov = of_ex.isMov.Read();

	//////////   Branch Unit  ///////////
	//pprint(2)<<"*** Branch Unit"<<endl;
	if (temp_isRet){
		branchPC = temp_A;
		//pprint(2)<<"branchPC: 0x"<<hex<<branchPC<<" (operand1)"<<endl;
	}
	else{
		branchPC = temp_branchTarget;
		//pprint(2)<<"branchPC: 0x"<<hex<<branchPC<<" (branchTarget)"<<endl;
	}
	

	if (temp_isUbranch){
		isBranchTaken = true;
		//pprint(2)<<"isBranchTaken: True"<<endl;
	}
	else if(temp_isBeq && eq) {
		isBranchTaken = true;
		//pprint(2)<<"isBranchTaken: True"<<endl;
	}
	else if(temp_isBgt && gt){
		isBranchTaken = true;
		//pprint(2)<<"isBranchTaken: True"<<endl;
	}
	else{
		isBranchTaken = false;
		//pprint(2)<<"isBranchTaken: False"<<endl;
	}


	//////////   ALU  ///////////

	//pprint(2)<<endl<<"*** ALU"<<endl;

	unsigned int temp_aluResult = 0;
	
	//pprint(2)<<"A: "<<dec<<temp_A<<endl;
	//pprint(2)<<"B: "<<dec<<temp_B<<endl;


	if (temp_isAdd){
		//pprint(2)<<"ADD operation"<<endl;
		temp_aluResult = temp_A + temp_B;
		
	}
	if (temp_isSub){		
		//pprint(2)<<"SUB operation"<<endl;
		temp_aluResult = temp_A + (~temp_B) + 1;
	}
	if (temp_isCmp){
		//pprint(2)<<"CMP operation"<<endl;
		if (temp_A +(~temp_B)+1  == 0){
			eq = true;
			gt = false;
			//pprint(2)<<"Equal"<<endl;
		}
		else if (inst_bitset(temp_A+(~temp_B)+1,32,32)   == 0){
			gt = true;
			eq = false;
			//pprint(2)<<"Greator than"<<endl;
		}
		else {
			eq = false;
			gt = false;
		}
		
	}

	if (temp_isMul){
		//pprint(2)<<"MUL operation"<<endl;
		temp_aluResult = (unsigned int)( (signed int)temp_A * (signed int)temp_B );
	}

	if (temp_isDiv){
		//pprint(2)<<"DIV operation"<<endl;
		temp_aluResult = (unsigned int)( (signed int)temp_A / (signed int)temp_B );
	}

	if (temp_isMod){
		//pprint(2)<<"MOD operation"<<endl;
		temp_aluResult = (unsigned int)( (signed int)temp_A % (signed int)temp_B );
	}

	if (temp_isLsl){
		//pprint(2)<<"LSL operation"<<endl;
		temp_aluResult = temp_A << temp_B;
	}

	if (temp_isLsr){
		//pprint(2)<<"LSR operation"<<endl;
		temp_aluResult = temp_A >> temp_B;
	}

	if (temp_isAsr){
		//pprint(2)<<"ASR operation"<<endl;
		temp_aluResult = temp_A;
		unsigned int count = temp_B;
		while(count){
			if (temp_aluResult>>31 == 1){
				temp_aluResult = temp_aluResult>>1;
				temp_aluResult = temp_aluResult | 0x80000000;
			}
			else{
				temp_aluResult = temp_aluResult>>1;
			}		

			count--;
		}
	}

	if (temp_isOr){
		//pprint(2)<<"OR operation"<<endl;
		temp_aluResult = temp_A | temp_B;
	}

	// NOT implemented using 1's Complement
	if (temp_isNot){
		//pprint(2)<<"NOT operation"<<endl;
		temp_aluResult = ~temp_B;
	}

	if (temp_isAnd){
		//pprint(2)<<"AND operation"<<endl;
		temp_aluResult = temp_A & temp_B;
	}

	if (temp_isMov){
		//pprint(2)<<"MOV operation"<<endl;
		temp_aluResult = temp_B;
	}

	//pprint(2)<<"aluResult: "<<dec<<temp_aluResult<<" (0x"<<hex<<temp_aluResult<<")"<<endl;

	ex_ma.PC.Write(temp_PC);
	ex_ma.instruction_word.Write(temp_instruction_word);

	ex_ma.aluResult.Write(temp_aluResult);
	ex_ma.operand2.Write(temp_operand2);

	ex_ma.isSt.Write(temp_isSt);
	ex_ma.isLd.Write(temp_isLd);
	ex_ma.isBeq.Write(temp_isBeq);
	ex_ma.isBgt.Write(temp_isBgt);
	ex_ma.isRet.Write(temp_isRet);
	ex_ma.isImmediate.Write(temp_isImmediate);
	ex_ma.isWb.Write(temp_isWb);
	ex_ma.isUbranch.Write(temp_isUbranch);
	ex_ma.isCall.Write(temp_isCall);
	ex_ma.isAdd.Write(temp_isAdd);
	ex_ma.isSub.Write(temp_isSub);
	ex_ma.isCmp.Write(temp_isCmp);
	ex_ma.isMul.Write(temp_isMul);
	ex_ma.isDiv.Write(temp_isDiv);
	ex_ma.isMod.Write(temp_isMod);
	ex_ma.isLsl.Write(temp_isLsl);
	ex_ma.isLsr.Write(temp_isLsr);
	ex_ma.isAsr.Write(temp_isAsr);
	ex_ma.isOr.Write(temp_isOr);
	ex_ma.isAnd.Write(temp_isAnd);
	ex_ma.isNot.Write(temp_isNot);
	ex_ma.isMov.Write(temp_isMov);

	ex_ma.ForwardBubble(bubble_inst);

}

//perform the memory operation
void Core::mem_access() {
	//pprint(2)<<endl<<"!--------- MEMORY ACCESS ---------!"<<endl<<endl;
	
	bool bubble_inst = ex_ma.bubble.Read();

	if (bubble_inst){
		//pprint(2)<<"<<<< Pipeline Bubble >>>>"<<endl<<endl;
	}

	unsigned int temp_PC = ex_ma.PC.Read();
	unsigned int temp_instruction_word = ex_ma.instruction_word.Read();

	unsigned int temp_aluResult = ex_ma.aluResult.Read();
	unsigned int temp_operand2 = ex_ma.operand2.Read();

	bool temp_isSt = ex_ma.isSt.Read();
	bool temp_isLd = ex_ma.isLd.Read();
	bool temp_isBeq = ex_ma.isBeq.Read();
	bool temp_isBgt = ex_ma.isBgt.Read();
	bool temp_isRet = ex_ma.isRet.Read();
	bool temp_isImmediate = ex_ma.isImmediate.Read();
	bool temp_isWb = ex_ma.isWb.Read();
	bool temp_isUbranch = ex_ma.isUbranch.Read();
	bool temp_isCall = ex_ma.isCall.Read();
	bool temp_isAdd = ex_ma.isAdd.Read();
	bool temp_isSub = ex_ma.isSub.Read();
	bool temp_isCmp = ex_ma.isCmp.Read();
	bool temp_isMul = ex_ma.isMul.Read();
	bool temp_isDiv = ex_ma.isDiv.Read();
	bool temp_isMod = ex_ma.isMod.Read();
	bool temp_isLsl = ex_ma.isLsl.Read();
	bool temp_isLsr = ex_ma.isLsr.Read();
	bool temp_isAsr = ex_ma.isAsr.Read();
	bool temp_isOr = ex_ma.isOr.Read();
	bool temp_isAnd = ex_ma.isAnd.Read();
	bool temp_isNot = ex_ma.isNot.Read();
	bool temp_isMov = ex_ma.isMov.Read();

	unsigned int temp_mar = temp_aluResult;
	unsigned int temp_mdr = temp_operand2;

	unsigned int temp_ldResult;

	if (temp_isLd){
		//pprint(2)<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

		//pprint(2)<<"Reading from Memory at address 0x"<<hex<<temp_aluResult<<endl;
		if (temp_mar >= 0 && temp_mar <= MEM_CAPACITY - sizeof(unsigned int)){
			temp_ldResult = MEM.Read(temp_mar);
		}
		
	}
	else if (temp_isSt){
		//pprint(2)<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

		//pprint(2)<<"Writing to Memory at address 0x"<<hex<<temp_aluResult<<" with data "<<dec<<temp_mdr<<endl;
		if (temp_mar >= 0 && temp_mar <= MEM_CAPACITY - sizeof(unsigned int)){
			MEM.Write(temp_mar,temp_mdr);
		}
	}
	else {
		//pprint(2)<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

		//pprint(2)<<"Memory unit Disabled"<<endl;
	}

	ma_rw.PC.Write(temp_PC);
	ma_rw.instruction_word.Write(temp_instruction_word);

	ma_rw.ldResult.Write(temp_ldResult);
	ma_rw.aluResult.Write(temp_aluResult);

	ma_rw.isSt.Write(temp_isSt);
	ma_rw.isLd.Write(temp_isLd);
	ma_rw.isBeq.Write(temp_isBeq);
	ma_rw.isBgt.Write(temp_isBgt);
	ma_rw.isRet.Write(temp_isRet);
	ma_rw.isImmediate.Write(temp_isImmediate);
	ma_rw.isWb.Write(temp_isWb);
	ma_rw.isUbranch.Write(temp_isUbranch);
	ma_rw.isCall.Write(temp_isCall);
	ma_rw.isAdd.Write(temp_isAdd);
	ma_rw.isSub.Write(temp_isSub);
	ma_rw.isCmp.Write(temp_isCmp);
	ma_rw.isMul.Write(temp_isMul);
	ma_rw.isDiv.Write(temp_isDiv);
	ma_rw.isMod.Write(temp_isMod);
	ma_rw.isLsl.Write(temp_isLsl);
	ma_rw.isLsr.Write(temp_isLsr);
	ma_rw.isAsr.Write(temp_isAsr);
	ma_rw.isOr.Write(temp_isOr);
	ma_rw.isAnd.Write(temp_isAnd);
	ma_rw.isNot.Write(temp_isNot);
	ma_rw.isMov.Write(temp_isMov);

	ma_rw.ForwardBubble(bubble_inst);
	
}
//writes the results back to register file
void Core::write_back() {
	//pprint(2)<<endl<<"!--------- WRITE BACK ---------!"<<endl<<endl;
	
	bool bubble_inst = ma_rw.bubble.Read();

	if (bubble_inst){
		//pprint(2)<<"<<<< Pipeline Bubble >>>>"<<endl<<endl;
	}

	unsigned int temp_PC = ma_rw.PC.Read();
	unsigned int temp_instruction_word = ma_rw.instruction_word.Read();

	unsigned int temp_ldResult = ma_rw.ldResult.Read();
	unsigned int temp_aluResult = ma_rw.aluResult.Read();

	bool temp_isSt = ma_rw.isSt.Read();
	bool temp_isLd = ma_rw.isLd.Read();
	bool temp_isBeq = ma_rw.isBeq.Read();
	bool temp_isBgt = ma_rw.isBgt.Read();
	bool temp_isRet = ma_rw.isRet.Read();
	bool temp_isImmediate = ma_rw.isImmediate.Read();
	bool temp_isWb = ma_rw.isWb.Read();
	bool temp_isUbranch = ma_rw.isUbranch.Read();
	bool temp_isCall = ma_rw.isCall.Read();
	bool temp_isAdd = ma_rw.isAdd.Read();
	bool temp_isSub = ma_rw.isSub.Read();
	bool temp_isCmp = ma_rw.isCmp.Read();
	bool temp_isMul = ma_rw.isMul.Read();
	bool temp_isDiv = ma_rw.isDiv.Read();
	bool temp_isMod = ma_rw.isMod.Read();
	bool temp_isLsl = ma_rw.isLsl.Read();
	bool temp_isLsr = ma_rw.isLsr.Read();
	bool temp_isAsr = ma_rw.isAsr.Read();
	bool temp_isOr = ma_rw.isOr.Read();
	bool temp_isAnd = ma_rw.isAnd.Read();
	bool temp_isNot = ma_rw.isNot.Read();
	bool temp_isMov = ma_rw.isMov.Read();

	unsigned int temp_result;
	unsigned int temp_addr;

	if (temp_isWb){

		//pprint(2)<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

		if (temp_isLd){
			temp_result = temp_ldResult;
			//pprint(2)<<"Writing data "<<dec<<temp_result;
			//pprint(2)<<" (ldResult)";
		}
		else if (temp_isCall){
			temp_result = temp_PC + 4;
			//pprint(2)<<"Writing data "<<dec<<temp_result;
			//pprint(2)<<" (PC + 4)";
		}
		else {
			temp_result = temp_aluResult;
			//pprint(2)<<"Writing data "<<dec<<temp_result;
			//pprint(2)<<" (aluResult)";
		}

		if (temp_isCall){			
			temp_addr = 15;
		}
		else {
			temp_addr = inst_bitset(temp_instruction_word,23,26);
		}
	
		//pprint(2)<<" to register R"<<dec<<temp_addr<<endl;
		R[temp_addr] = temp_result;
		fprint(1)<<", R"<<dec<<temp_addr<<" = "<<dec<<temp_result;

	}
	else {
		//pprint(2)<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

		//pprint(2)<<"Write Back Disabled"<<endl;
	}
	fprint(1)<<endl;

	fprint(2)<<"   ";
	if (temp_isSt) {
	fprint(2)<<" isSt";
	}
	if (temp_isLd) {
		fprint(2)<<" isLd";
	}
	if (temp_isBeq) {
		fprint(2)<<" isBeq";
	}
	if (temp_isBgt) {
		fprint(2)<<" isBgt";
	}
	if (temp_isRet) {
		fprint(2)<<" isRet";
	}
	if (temp_isImmediate) {
		fprint(2)<<" isImmediate";
	}
	if (temp_isWb) {
		fprint(2)<<" isWb";
	}
	if (temp_isUbranch) {
		fprint(2)<<" isUbranch";
	}
	if (temp_isCall) {
		fprint(2)<<" isCall";
	}
	if (temp_isAdd) {
		fprint(2)<<" isAdd";
	}
	if (temp_isSub) {
		fprint(2)<<" isSub";
	}
	if (temp_isCmp) {
		fprint(2)<<" isCmp";
	}
	if (temp_isMul) {
		fprint(2)<<" isMul";
	}
	if (temp_isDiv) {
		fprint(2)<<" isDiv";
	}
	if (temp_isMod) {
		fprint(2)<<" isMod";
	}
	if (temp_isLsl) {
		fprint(2)<<" isLsl";
	}
	if (temp_isLsr) {
		fprint(2)<<" isLsr";
	}
	if (temp_isAsr) {
		fprint(2)<<" isAsr";
	}
	if (temp_isOr) {
		fprint(2)<<" isOr";
	}
	if (temp_isAnd) {
		fprint(2)<<" isAnd";
	}
	if (temp_isNot) {
		fprint(2)<<" isNot";
	}
	if (temp_isMov) {
		fprint(2)<<" isMov";
	}

	fprint(2)<<endl;

	if (isBranchTaken) {
		fprint(2)<<"    isBranchTaken branchPC=0x"<<hex<<branchPC<<endl;
	}


}

bool Core::checkValidPC(unsigned int testPC){
	if (testPC < 0 || testPC > MEM_CAPACITY - sizeof(unsigned int)){
		return false;
	}
	unsigned int testInst = MEM.Read(testPC);

	if (inst_bitset(testInst,28, 32) < 21) {
		return true;
	}

	return false;

}

unsigned int Core::inst_bitset(unsigned int inst_word, unsigned int start, unsigned int end){
	inst_word = inst_word<<(32-end);
	inst_word = inst_word>>(32-end + start-1);
	return inst_word;
}

bool Core::check_data_conflict(PipelineRegister& A, PipelineRegister& B){
	unsigned int A_instruction_word = A.instruction_word.Read();

	unsigned int A_opcode1 = inst_bitset(A_instruction_word, 28, 28);
	unsigned int A_opcode2 = inst_bitset(A_instruction_word, 29, 29);
	unsigned int A_opcode3 = inst_bitset(A_instruction_word, 30, 30);
	unsigned int A_opcode4 = inst_bitset(A_instruction_word, 31, 31);
	unsigned int A_opcode5 = inst_bitset(A_instruction_word, 32, 32);

	bool A_bubble_inst = A.bubble.Read();

	if (A_bubble_inst){
		return false;	//A is bubble
	}
	if (A_opcode5 == 0 && A_opcode4 == 1 && A_opcode3 == 1 && A_opcode2 == 0 && A_opcode1 == 1){
		return false;	//A is nop
	}
	if (A_opcode5 == 1 && A_opcode4 == 0 && A_opcode3 == 0 && A_opcode2 == 1 && A_opcode1 == 0){
		return false;	//A is b
	}
	if (A_opcode5 == 1 && A_opcode4 == 0 && A_opcode3 == 0 && A_opcode2 == 0 && A_opcode1 == 0){
		return false;	//A is beq
	}
	if (A_opcode5 == 1 && A_opcode4 == 0 && A_opcode3 == 0 && A_opcode2 == 0 && A_opcode1 == 1){
		return false;	//A is bgt
	}
	if (A_opcode5 == 1 && A_opcode4 == 0 && A_opcode3 == 0 && A_opcode2 == 1 && A_opcode1 == 1){
		return false;	//A is call
	}

	unsigned int B_instruction_word = B.instruction_word.Read();

	unsigned int B_opcode1 = inst_bitset(B_instruction_word, 28, 28);
	unsigned int B_opcode2 = inst_bitset(B_instruction_word, 29, 29);
	unsigned int B_opcode3 = inst_bitset(B_instruction_word, 30, 30);
	unsigned int B_opcode4 = inst_bitset(B_instruction_word, 31, 31);
	unsigned int B_opcode5 = inst_bitset(B_instruction_word, 32, 32);

	bool B_bubble_inst = B.bubble.Read();

	if (B_bubble_inst){
		return false;	//B is bubble
	}
	if (B_opcode5 == 0 && B_opcode4 == 1 && B_opcode3 == 1 && B_opcode2 == 0 && B_opcode1 == 1){
		return false;	//B is nop
	}
	if (B_opcode5 == 0 && B_opcode4 == 0 && B_opcode3 == 1 && B_opcode2 == 0 && B_opcode1 == 1){
		return false;	//B is cmp
	}
	if (B_opcode5 == 0 && B_opcode4 == 1 && B_opcode3 == 1 && B_opcode2 == 1 && B_opcode1 == 1){
		return false;	//B is st
	}
	if (B_opcode5 == 1 && B_opcode4 == 0 && B_opcode3 == 0 && B_opcode2 == 1 && B_opcode1 == 0){
		return false;	//B is b
	}
	if (B_opcode5 == 1 && B_opcode4 == 0 && B_opcode3 == 0 && B_opcode2 == 0 && B_opcode1 == 0){
		return false;	//B is beq
	}
	if (B_opcode5 == 1 && B_opcode4 == 0 && B_opcode3 == 0 && B_opcode2 == 0 && B_opcode1 == 1){
		return false;	//B is bgt
	}
	if (B_opcode5 == 1 && B_opcode4 == 0 && B_opcode3 == 1 && B_opcode2 == 0 && B_opcode1 == 0){
		return false;	//B is ret
	}

	unsigned int A_rs1 = inst_bitset(A_instruction_word, 19,22);
	unsigned int A_rs2 = inst_bitset(A_instruction_word, 15,18);
	unsigned int A_rd = inst_bitset(A_instruction_word, 23,26);

	unsigned int src1 = A_rs1;
	unsigned int src2 = A_rs2;

	if (A_opcode5 == 0 && A_opcode4 == 1 && A_opcode3 == 1 && A_opcode2 == 1 && A_opcode1 == 1){
		//A is st
		src2 = A_rd;
	}
	if (A_opcode5 == 1 && A_opcode4 == 0 && A_opcode3 == 1 && A_opcode2 == 0 && A_opcode1 == 0){
		//A is ret
		src1 = 15;
	}

	unsigned int B_rd = inst_bitset(B_instruction_word, 23,26);

	unsigned int dest = B_rd;

	if (B_opcode5 == 1 && B_opcode4 == 0 && B_opcode3 == 0 && B_opcode2 == 1 && B_opcode1 == 1){
		//B is call
		dest = 15;
	}

	unsigned int A_I_bit = inst_bitset(A_instruction_word, 27, 27);

	bool hasSrc1 = true;

	if (A_opcode5 == 0 && A_opcode4 == 1 && A_opcode3 == 0 && A_opcode2 == 0 && A_opcode1 == 1){
		//A is mov
		hasSrc1 = false;
	}
	if (A_opcode5 == 0 && A_opcode4 == 1 && A_opcode3 == 0 && A_opcode2 == 0 && A_opcode1 == 0){
		//A is not
		hasSrc1 = false;
	}

	bool hasSrc2 = true;
	if (!(A_opcode5 == 0 && A_opcode4 == 1 && A_opcode3 == 1 && A_opcode2 == 1 && A_opcode1 == 1)){
		//A is NOT st
		if (A_I_bit == 1){
			hasSrc2 = false;
		}
	}

	if (hasSrc1 && (src1 == dest)){
		return true;
	}

	if (hasSrc2 && (src2 == dest)){
		return true;
	}

	return false;
}

bool Core::detect_data_dependency(){

	bool isDataDependency = false;

	if (pipeline){		
		if (check_data_conflict(if_of, of_ex)){
			////pprint(2)<<"Data Dependency between DECODE and EXECUTE"<<endl;
			isDataDependency = true;
		}
		else if (check_data_conflict(if_of, ex_ma)){
			////pprint(2)<<"Data Dependency between DECODE and MEMORY ACCESS"<<endl;
			isDataDependency = true;
		}
		else if (check_data_conflict(if_of, ma_rw)){
			////pprint(2)<<"Data Dependency between DECODE and MEMORY ACCESS"<<endl;
			isDataDependency = true;
		}
	}

	return isDataDependency;

}

bool Core::detect_control_dependency(){

	return (pipeline && isBranchTaken);	
}