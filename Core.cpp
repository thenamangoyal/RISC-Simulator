#include "Core.h"

using namespace std;


Core::Core(bool pipe){
	INST_MAX = 0;
	MEM = new Memory(MEM_CAPACITY);
	pipeline = pipe;

	if_of = new PipelineRegister(pipe);
	of_ex = new PipelineRegister(pipe);
	ex_ma = new PipelineRegister(pipe);
	ma_rw = new PipelineRegister(pipe);

	PC = new Register<unsigned int>(true);
}

Core::~Core(){
	delete MEM;
	
	delete if_of;
	delete of_ex;
	delete ex_ma;
	delete ma_rw;

	delete PC;
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

void Core::write_state() {
	ofstream out_file;
	out_file.open("STATE_OUT.mem",ios::out | ios::trunc);

	out_file<<"Registers"<<endl<<endl;

	for(int i=0; i<16; i++){
		out_file<<"R"<<dec<<i<<" : "<<dec<<R[i]<<endl;
	}

	out_file<<endl;
	out_file<<"PC : 0x"<<hex<<PC->Read()<<endl;
	out_file<<endl;

	if (eq){
		out_file<<"Flags.eq : True"<<endl;
	}
	else {
		out_file<<"Flags.eq : False"<<endl;
	}

	if (gt){
		out_file<<"Flags.gt : True"<<endl;
	}
	else {
		out_file<<"Flags.gt : False"<<endl;
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
	PC->Write(0);
	PC->clock();

	if_of->bubble->Write(pipeline);
	of_ex->bubble->Write(pipeline);
	ex_ma->bubble->Write(pipeline);
	ma_rw->bubble->Write(pipeline);

	if_of->bubble->clock();
	of_ex->bubble->clock();
	ex_ma->bubble->clock();
	ma_rw->bubble->clock();

	eq = false;
	gt = false;
	isBranchTaken = false;
	branchPC = 0x0;
}


void Core::run_simplesim(){


	if (pipeline) {
		cout<<"Pipeline Based"<<endl;
	}
	else {
		cout<<"Single Cycle Based"<<endl;
	}

	int counter = 0;
	while (PC->Read() != INST_MAX){
		cout<<"========================"<<endl;
		cout<<"CYCLE "<<dec<<counter+1<<endl;
		cout<<"========================"<<endl;
		fetch_begin();
		decode();
		execute();
		mem_access();
		write_back();
		fetch_end();

		PC->clock();
		if_of->clock();
		of_ex->clock();
		ex_ma->clock();
		ma_rw->clock();

		counter++;
	}
	if (INST_MAX && pipeline){
		for(int i=0; i<4; i++){
		cout<<"========================"<<endl;
		cout<<"CYCLE "<<dec<<counter+1<<endl;
		cout<<"========================"<<endl;
		cout<<"Flushing pipeline"<<endl;
		decode();
		execute();
		mem_access();
		write_back();

		if_of->bubble->Write(true);

		if_of->clock();
		of_ex->clock();
		ex_ma->clock();
		ma_rw->clock();

		counter++;
		}
	}
}

//reads from the instruction memory
void Core::fetch_begin() {
	cout<<endl<<"!--------- FETCH ---------!"<<endl<<endl;

	unsigned int temp_PC = PC->Read();
	unsigned int temp_instruction_word = MEM->Read(PC->Read());	

	cout<<"Instruction 0x"<<hex<<temp_instruction_word<<" read at address 0x"<<hex<<temp_PC<<endl;
	//cout<<bitset<32> (temp_instruction_word)<<" : Instruction encoding"<<endl;

	if_of->PC->Write(temp_PC);
	if_of->instruction_word->Write(temp_instruction_word);
	if_of->bubble->Write(false);
	
}


//updates the instruction register
void Core::fetch_end() {

	unsigned int temp_PC = PC->Read();

	cout<<endl;
	if (isBranchTaken){
		PC->Write(branchPC);
		cout<<"New PC = 0x"<<hex<<branchPC<<" (branchPC)"<<endl;
	}
	else {
		
		PC->Write(temp_PC + 4);
		cout<<"New PC = 0x"<<hex<<(temp_PC + 4)<<" (PC + 4)"<<endl;
	}
	cout<<endl;

}

//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void Core::decode() {
	cout<<endl<<"!--------- DECODE ---------!"<<endl<<endl;

	bool bubble_inst = if_of->bubble->Read();

	if (bubble_inst){
		cout<<"Pipeline bubble instruction"<<endl;
	}
	else {
	
		unsigned int temp_PC = if_of->PC->Read();
		unsigned int temp_instruction_word = if_of->instruction_word->Read();

		cout<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

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
		cout<<"Control Signals"<<endl;
		if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1 && opcode1 == 1){
			temp_isSt = true;
			cout<<"isSt ";
		}
		else{
			temp_isSt = false;
		}

		if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1 && opcode1 == 0){
			temp_isLd = true;
			cout<<"isLd ";
		}
		else{
			temp_isLd = false;
		}
		
		if(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 0){
			temp_isBeq = true;
			cout<<"isBeq ";
		}
		else{
			temp_isBeq = false;
		}

		if(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 1){
			temp_isBgt = true;
			cout<<"isBgt ";
		}
		else{
			temp_isBgt = false;
		}

		if(opcode5 == 1 && opcode4 == 0 && opcode3 == 1 && opcode2 == 0 && opcode1 == 0){
			temp_isRet = true;
			cout<<"isRet ";
		}
		else{
			temp_isRet = false;
		}

		if(I_bit == 1){
			temp_isImmediate = true;
			cout<<"isImmediate ";
		}
		else{
			temp_isImmediate = false;
		}

		if( !(opcode5 == 1 || ( opcode5 == 0 && opcode3 == 1 && opcode1 == 1 && ( opcode4 == 1 || opcode2 == 0) ) ) 	|| 	(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1)  ){
			temp_isWb = true;
			cout<<"isWb ";
		}
		else{
			temp_isWb = false;
		}

		if( opcode5 == 1 && opcode4 == 0 && (	(opcode3 == 0 && opcode2 == 1) || (opcode3 == 1 && opcode2 == 0 && opcode1 == 0) ) 	){
			temp_isUbranch = true;
			cout<<"isUbranch ";
		}
		else{
			temp_isUbranch = false;
		}

		if(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1){
			temp_isCall = true;
			cout<<"isCall ";
		}
		else{
			temp_isCall = false;
		}

		if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 0){
			temp_isAdd = true;
			cout<<"isAdd ";
		}
		else if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1){
			temp_isAdd = true;
			cout<<"isAdd ";
		}
		else{
			temp_isAdd = false;
		}

		if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 1){
			temp_isSub = true;
			cout<<"isSub ";
		}
		else{
			temp_isSub = false;
		}

		if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 0 && opcode1 == 1){
			temp_isCmp = true;
			cout<<"isCmp ";
		}
		else{
			temp_isCmp = false;
		}

		if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 0){
			temp_isMul = true;
			cout<<"isMul ";
		}
		else{
			temp_isMul = false;
		}

		if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1){
			temp_isDiv = true;
			cout<<"isDiv ";
		}
		else{
			temp_isDiv = false;
		}

		if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 0 && opcode1 == 0){
			temp_isMod = true;
			cout<<"isMod ";
		}
		else{
			temp_isMod = false;
		}

		if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 1 && opcode1 == 0){
			temp_isLsl = true;
			cout<<"isLsl ";
		}
		else{
			temp_isLsl = false;
		}

		if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1){
			temp_isLsr = true;
			cout<<"islsr ";
		}
		else{
			temp_isLsr = false;
		}

		if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 0 && opcode1 == 0){
			temp_isAsr = true;
			cout<<"isAsr ";
		}
		else{
			temp_isAsr = false;
		}

		if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 1 && opcode1 == 1){
			temp_isOr = true;
			cout<<"isOr ";
		}
		else{
			temp_isOr = false;
		}

		if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 1 && opcode1 == 0){
			temp_isAnd = true;
			cout<<"isAnd ";
		}
		else{
			temp_isAnd = false;
		}

		if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 0 && opcode1 == 0){
			temp_isNot = true;
			cout<<"isNot ";
		}
		else{
			temp_isNot = false;
		}

		if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 0 && opcode1 == 1){
			temp_isMov = true;
			cout<<"isMov ";
		}
		else{
			temp_isMov = false;
		}
		cout<<endl;


		//////////   immx calculation  ///////////
		cout<<endl;
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
			cout<<"Immediate is "<<dec<<temp_immx<<" (0x"<<hex<<temp_immx<<")"<<endl;
		}
		else if (temp_u == 1){
			temp_immx = temp_imm;
			cout<<"Immediate is "<<dec<<temp_immx<<" (0x"<<hex<<temp_immx<<") and is Unsigned"<<endl;
		}
		else{
			temp_immx = temp_imm<<16;
			cout<<"Immediate is "<<dec<<temp_immx<<" (0x"<<hex<<temp_immx<<") ans is High"<<endl;
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
		cout<<endl<<"branchTarget temp_is 0x"<<hex<<temp_branchTarget<<endl;


		//////////   Reading Register File  ///////////
		unsigned int temp_operand1;
		unsigned int temp_operand2;
		unsigned int temp_rd = inst_bitset(temp_instruction_word, 23,26);
		unsigned int temp_rs1 = inst_bitset(temp_instruction_word, 19,22);
		unsigned int temp_rs2 = inst_bitset(temp_instruction_word, 15,18);

		cout<<endl<<"rd: R"<<dec<<temp_rd<<", rs1: R"<<dec<<temp_rs1<<", rs2: R"<<dec<<temp_rs2<<endl;

		if (temp_isRet){
			temp_operand1 = R[15];
			cout<<"Operand1: "<<dec<<temp_operand1<<" (Read from ra OR R15)"<<endl;
		}
		else{
			temp_operand1 = R[temp_rs1];
			cout<<"Operand1: "<<dec<<temp_operand1<<" (Read from rs1)"<<endl;
		}
		

		if (temp_isSt){
			temp_operand2 = R[temp_rd];
			cout<<"Operand2: "<<dec<<temp_operand2<<" (Read from rd)"<<endl;
		}
		else{
			temp_operand2 = R[temp_rs2];
			cout<<"Operand2: "<<dec<<temp_operand2<<" (Read from rs2)"<<endl;
		}

		unsigned int temp_A = temp_operand1;
		unsigned int temp_B;

		if (temp_isImmediate){
			temp_B = temp_immx;
			cout<<"B: "<<dec<<temp_B<<" (immx)"<<endl;
		}
		else {
			temp_B = temp_operand2;
			cout<<"B: "<<dec<<temp_B<<" (operand2)"<<endl;
		}

		of_ex->PC->Write(temp_PC);
		of_ex->instruction_word->Write(temp_instruction_word);

		of_ex->branchTarget->Write(temp_branchTarget);

		of_ex->A->Write(temp_A);	
		of_ex->B->Write(temp_B);
		of_ex->operand2->Write(temp_operand2);

		of_ex->isSt->Write(temp_isSt);
		of_ex->isLd->Write(temp_isLd);
		of_ex->isBeq->Write(temp_isBeq);
		of_ex->isBgt->Write(temp_isBgt);
		of_ex->isRet->Write(temp_isRet);
		of_ex->isImmediate->Write(temp_isImmediate);
		of_ex->isWb->Write(temp_isWb);
		of_ex->isUbranch->Write(temp_isUbranch);
		of_ex->isCall->Write(temp_isCall);
		of_ex->isAdd->Write(temp_isAdd);
		of_ex->isSub->Write(temp_isSub);
		of_ex->isCmp->Write(temp_isCmp);
		of_ex->isMul->Write(temp_isMul);
		of_ex->isDiv->Write(temp_isDiv);
		of_ex->isMod->Write(temp_isMod);
		of_ex->isLsl->Write(temp_isLsl);
		of_ex->isLsr->Write(temp_isLsr);
		of_ex->isAsr->Write(temp_isAsr);
		of_ex->isOr->Write(temp_isOr);
		of_ex->isAnd->Write(temp_isAnd);
		of_ex->isNot->Write(temp_isNot);
		of_ex->isMov->Write(temp_isMov);

	}

	of_ex->bubble->Write(bubble_inst);

}

//executes the ALU operation based on ALUop
void Core::execute() {
	cout<<endl<<"!--------- EXECUTE ---------!"<<endl<<endl;

	bool bubble_inst = of_ex->bubble->Read();

	if (bubble_inst){
		cout<<"Pipeline bubble instruction"<<endl;
	}
	else {
	
		unsigned int temp_PC = of_ex->PC->Read();
		unsigned int temp_instruction_word = of_ex->instruction_word->Read();

		cout<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

		unsigned int temp_branchTarget = of_ex->branchTarget->Read();

		unsigned int temp_A = of_ex->A->Read();	
		unsigned int temp_B = of_ex->B->Read();
		unsigned int temp_operand2 = of_ex->operand2->Read();

		bool temp_isSt = of_ex->isSt->Read();
		bool temp_isLd = of_ex->isLd->Read();
		bool temp_isBeq = of_ex->isBeq->Read();
		bool temp_isBgt = of_ex->isBgt->Read();
		bool temp_isRet = of_ex->isRet->Read();
		bool temp_isImmediate = of_ex->isImmediate->Read();
		bool temp_isWb = of_ex->isWb->Read();
		bool temp_isUbranch = of_ex->isUbranch->Read();
		bool temp_isCall = of_ex->isCall->Read();
		bool temp_isAdd = of_ex->isAdd->Read();
		bool temp_isSub = of_ex->isSub->Read();
		bool temp_isCmp = of_ex->isCmp->Read();
		bool temp_isMul = of_ex->isMul->Read();
		bool temp_isDiv = of_ex->isDiv->Read();
		bool temp_isMod = of_ex->isMod->Read();
		bool temp_isLsl = of_ex->isLsl->Read();
		bool temp_isLsr = of_ex->isLsr->Read();
		bool temp_isAsr = of_ex->isAsr->Read();
		bool temp_isOr = of_ex->isOr->Read();
		bool temp_isAnd = of_ex->isAnd->Read();
		bool temp_isNot = of_ex->isNot->Read();
		bool temp_isMov = of_ex->isMov->Read();

		//////////   Branch Unit  ///////////
		cout<<"*** Branch Unit"<<endl;
		if (temp_isRet){
			branchPC = temp_A;
			cout<<"branchPC: 0x"<<hex<<branchPC<<" (operand1)"<<endl;
		}
		else{
			branchPC = temp_branchTarget;
			cout<<"branchPC: 0x"<<hex<<branchPC<<" (branchTarget)"<<endl;
		}
		

		if (temp_isUbranch){
			isBranchTaken = true;
			cout<<"isBranchTaken: True"<<endl;
		}
		else if(temp_isBeq && eq) {
			isBranchTaken = true;
			cout<<"isBranchTaken: True"<<endl;
		}
		else if(temp_isBgt && gt){
			isBranchTaken = true;
			cout<<"isBranchTaken: True"<<endl;
		}
		else{
			isBranchTaken = false;
			cout<<"isBranchTaken: False"<<endl;
		}


		//////////   ALU  ///////////

		cout<<endl<<"*** ALU"<<endl;

		unsigned int temp_aluResult;

		
		cout<<"A: "<<dec<<temp_A<<endl;
		cout<<"B: "<<dec<<temp_B<<endl;


		if (temp_isAdd){
			cout<<"ADD operation"<<endl;
			temp_aluResult = temp_A + temp_B;
			
		}
		if (temp_isSub){		
			cout<<"SUB operation"<<endl;
			temp_aluResult = temp_A + (~temp_B) + 1;
		}
		if (temp_isCmp){
			cout<<"CMP operation"<<endl;
			if (temp_A +(~temp_B)+1  == 0){
				eq = true;
				gt = false;
				cout<<"Equal"<<endl;
			}
			else if (inst_bitset(temp_A+(~temp_B)+1,32,32)   == 0){
				gt = true;
				eq = false;
				cout<<"Greator than"<<endl;
			}
			else {
				eq = false;
				gt = false;
			}
			
		}

		if (temp_isMul){
			cout<<"MUL operation"<<endl;
			temp_aluResult = temp_A * temp_B;
		}

		if (temp_isDiv){
			cout<<"DIV operation"<<endl;
			temp_aluResult = temp_A / temp_B;
		}

		if (temp_isMod){
			cout<<"MOD operation"<<endl;
			temp_aluResult = temp_A % temp_B;
		}

		if (temp_isLsl){
			cout<<"LSL operation"<<endl;
			temp_aluResult = temp_A << temp_B;
		}

		if (temp_isLsr){
			cout<<"LSR operation"<<endl;
			temp_aluResult = temp_A >> temp_B;
		}

		if (temp_isAsr){
			cout<<"ASR operation"<<endl;
			temp_aluResult = temp_A;
			unsigned int count = temp_B;
			while(count){
				if (temp_aluResult>>31 == 1){
					temp_aluResult>>1;
					temp_aluResult = temp_aluResult | 0x80000000;
				}
				else{
					temp_aluResult>>1;
				}		

				count--;
			}
		}

		if (temp_isOr){
			cout<<"OR operation"<<endl;
			temp_aluResult = temp_A | temp_B;
		}

		if (temp_isNot){
			cout<<"NOT operation"<<endl;
			temp_aluResult = ~temp_B;
		}

		if (temp_isAnd){
			cout<<"AND operation"<<endl;
			temp_aluResult = temp_A & temp_B;
		}

		if (temp_isMov){
			cout<<"MOV operation"<<endl;
			temp_aluResult = temp_B;
		}

		cout<<"aluResult: "<<dec<<temp_aluResult<<" (0x"<<hex<<temp_aluResult<<")"<<endl;

		ex_ma->PC->Write(temp_PC);
		ex_ma->instruction_word->Write(temp_instruction_word);

		ex_ma->aluResult->Write(temp_aluResult);
		ex_ma->operand2->Write(temp_operand2);

		ex_ma->isSt->Write(temp_isSt);
		ex_ma->isLd->Write(temp_isLd);
		ex_ma->isBeq->Write(temp_isBeq);
		ex_ma->isBgt->Write(temp_isBgt);
		ex_ma->isRet->Write(temp_isRet);
		ex_ma->isImmediate->Write(temp_isImmediate);
		ex_ma->isWb->Write(temp_isWb);
		ex_ma->isUbranch->Write(temp_isUbranch);
		ex_ma->isCall->Write(temp_isCall);
		ex_ma->isAdd->Write(temp_isAdd);
		ex_ma->isSub->Write(temp_isSub);
		ex_ma->isCmp->Write(temp_isCmp);
		ex_ma->isMul->Write(temp_isMul);
		ex_ma->isDiv->Write(temp_isDiv);
		ex_ma->isMod->Write(temp_isMod);
		ex_ma->isLsl->Write(temp_isLsl);
		ex_ma->isLsr->Write(temp_isLsr);
		ex_ma->isAsr->Write(temp_isAsr);
		ex_ma->isOr->Write(temp_isOr);
		ex_ma->isAnd->Write(temp_isAnd);
		ex_ma->isNot->Write(temp_isNot);
	}

	ex_ma->bubble->Write(bubble_inst);

}

//perform the memory operation
void Core::mem_access() {
	cout<<endl<<"!--------- MEMORY ACCESS ---------!"<<endl<<endl;

	bool bubble_inst = ex_ma->bubble->Read();

	if (bubble_inst){
		cout<<"Pipeline bubble instruction"<<endl;
	}
	else {

		unsigned int temp_PC = ex_ma->PC->Read();
		unsigned int temp_instruction_word = ex_ma->instruction_word->Read();

		cout<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

		unsigned int temp_aluResult = ex_ma->aluResult->Read();
		unsigned int temp_operand2 = ex_ma->operand2->Read();

		bool temp_isSt = ex_ma->isSt->Read();
		bool temp_isLd = ex_ma->isLd->Read();
		bool temp_isBeq = ex_ma->isBeq->Read();
		bool temp_isBgt = ex_ma->isBgt->Read();
		bool temp_isRet = ex_ma->isRet->Read();
		bool temp_isImmediate = ex_ma->isImmediate->Read();
		bool temp_isWb = ex_ma->isWb->Read();
		bool temp_isUbranch = ex_ma->isUbranch->Read();
		bool temp_isCall = ex_ma->isCall->Read();
		bool temp_isAdd = ex_ma->isAdd->Read();
		bool temp_isSub = ex_ma->isSub->Read();
		bool temp_isCmp = ex_ma->isCmp->Read();
		bool temp_isMul = ex_ma->isMul->Read();
		bool temp_isDiv = ex_ma->isDiv->Read();
		bool temp_isMod = ex_ma->isMod->Read();
		bool temp_isLsl = ex_ma->isLsl->Read();
		bool temp_isLsr = ex_ma->isLsr->Read();
		bool temp_isAsr = ex_ma->isAsr->Read();
		bool temp_isOr = ex_ma->isOr->Read();
		bool temp_isAnd = ex_ma->isAnd->Read();
		bool temp_isNot = ex_ma->isNot->Read();

		unsigned int temp_mar = mem_address(temp_aluResult);
		unsigned int temp_mdr = temp_operand2;

		unsigned int temp_ldResult;

		if (temp_isLd){
			cout<<"Reading from Memory at address 0x"<<hex<<temp_aluResult<<endl;
			temp_ldResult = MEM->Read(temp_mar);
		}
		else if (temp_isSt){
			cout<<"Writing to Memory at address 0x"<<hex<<temp_aluResult<<" with data "<<dec<<temp_mdr<<endl;
			MEM->Write(temp_mar,temp_mdr);
		}
		else {
			cout<<"Memory unit Disabled"<<endl;
		}

		ma_rw->PC->Write(temp_PC);
		ma_rw->instruction_word->Write(temp_instruction_word);

		ma_rw->ldResult->Write(temp_ldResult);
		ma_rw->aluResult->Write(temp_aluResult);

		ma_rw->isSt->Write(temp_isSt);
		ma_rw->isLd->Write(temp_isLd);
		ma_rw->isBeq->Write(temp_isBeq);
		ma_rw->isBgt->Write(temp_isBgt);
		ma_rw->isRet->Write(temp_isRet);
		ma_rw->isImmediate->Write(temp_isImmediate);
		ma_rw->isWb->Write(temp_isWb);
		ma_rw->isUbranch->Write(temp_isUbranch);
		ma_rw->isCall->Write(temp_isCall);
		ma_rw->isAdd->Write(temp_isAdd);
		ma_rw->isSub->Write(temp_isSub);
		ma_rw->isCmp->Write(temp_isCmp);
		ma_rw->isMul->Write(temp_isMul);
		ma_rw->isDiv->Write(temp_isDiv);
		ma_rw->isMod->Write(temp_isMod);
		ma_rw->isLsl->Write(temp_isLsl);
		ma_rw->isLsr->Write(temp_isLsr);
		ma_rw->isAsr->Write(temp_isAsr);
		ma_rw->isOr->Write(temp_isOr);
		ma_rw->isAnd->Write(temp_isAnd);
		ma_rw->isNot->Write(temp_isNot);
	}
	ma_rw->bubble->Write(bubble_inst);
}
//writes the results back to register file
void Core::write_back() {
	cout<<endl<<"!--------- WRITE BACK ---------!"<<endl<<endl;

	bool bubble_inst = ma_rw->bubble->Read();

	if (bubble_inst){
		cout<<"Pipeline bubble instruction"<<endl;
	}
	else {

		unsigned int temp_PC = ma_rw->PC->Read();
		unsigned int temp_instruction_word = ma_rw->instruction_word->Read();

		cout<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

		unsigned int temp_ldResult = ma_rw->ldResult->Read();
		unsigned int temp_aluResult = ma_rw->aluResult->Read();

		bool temp_isSt = ma_rw->isSt->Read();
		bool temp_isLd = ma_rw->isLd->Read();
		bool temp_isBeq = ma_rw->isBeq->Read();
		bool temp_isBgt = ma_rw->isBgt->Read();
		bool temp_isRet = ma_rw->isRet->Read();
		bool temp_isImmediate = ma_rw->isImmediate->Read();
		bool temp_isWb = ma_rw->isWb->Read();
		bool temp_isUbranch = ma_rw->isUbranch->Read();
		bool temp_isCall = ma_rw->isCall->Read();
		bool temp_isAdd = ma_rw->isAdd->Read();
		bool temp_isSub = ma_rw->isSub->Read();
		bool temp_isCmp = ma_rw->isCmp->Read();
		bool temp_isMul = ma_rw->isMul->Read();
		bool temp_isDiv = ma_rw->isDiv->Read();
		bool temp_isMod = ma_rw->isMod->Read();
		bool temp_isLsl = ma_rw->isLsl->Read();
		bool temp_isLsr = ma_rw->isLsr->Read();
		bool temp_isAsr = ma_rw->isAsr->Read();
		bool temp_isOr = ma_rw->isOr->Read();
		bool temp_isAnd = ma_rw->isAnd->Read();
		bool temp_isNot = ma_rw->isNot->Read();

		unsigned int temp_result;
		unsigned int temp_addr;

		if (temp_isWb){

			if (temp_isLd){
				temp_result = temp_ldResult;
				cout<<"Writing data "<<dec<<temp_result<<" (temp_ldResult)";
			}
			else if (temp_isCall){
				temp_result = temp_PC + 4;
				cout<<"Writing data "<<dec<<temp_result<<" (PC + 4)";
			}
			else {
				temp_result = temp_aluResult;
				cout<<"Writing data "<<dec<<temp_result<<" (aluResult)";
			}

			if (temp_isCall){			
				temp_addr = 15;
			}
			else {
				temp_addr = inst_bitset(temp_instruction_word,23,26);
			}
		
			cout<<" to register R"<<dec<<temp_addr<<endl;
			R[temp_addr] = temp_result;

		}
		else {
			cout<<"Write Back Disabled"<<endl;
		}
	}
}

unsigned int Core::mem_address(unsigned int data_address){
	return data_address + INST_MAX;
}

unsigned int Core::inst_bitset(unsigned int inst_word, unsigned int start, unsigned int end){
	inst_word = inst_word<<(32-end);
	inst_word = inst_word>>(32-end + start-1);
	return inst_word;
}

