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
	/*ofstream out_file;
	out_file.open("STATE_OUT.mem",ios::out | ios::trunc);

	out_file<<"Registers"<<endl<<endl;

	for(int i=0; i<16; i++){
		out_file<<"R"<<dec<<i<<" : "<<dec<<R[i]<<endl;
	}

	out_file<<endl;
	out_file<<"PC : 0x"<<hex<<PC<<endl;
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
  	
	out_file.close();*/
}

void Core::run_simplesim(){
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
}


void Core::reset_proc()
{
	for(int i=0;i<16;i++)
	{
		R[i] = 0;
	}
	R[14] = MEM_CAPACITY - INST_MAX; // Stack Pointer initialize
	PC = 0;
	eq = false;
	gt = false;
}

//reads from the instruction memory
void Core::fetch_begin() {
	cout<<endl<<"!--------- FETCH ---------!"<<endl<<endl;
	unsigned int temp_instruction_word = MEM->Read(PC->Read());	
	cout<<"Instruction 0x"<<hex<<temp_instruction_word<<" read at address 0x"<<hex<<PC->Read()<<endl;
	//cout<<bitset<32> (instruction_word)<<" : Instruction encoding"<<endl;

	if_of->PC->Write() = PC->Read();
	if_of->instruction_word->Write() = temp_instruction_word;
	
}


//updates the instruction register
void Core::fetch_end() {
	cout<<endl;
	if (isBranchTaken){
		PC = branchPC;
		cout<<"New PC = 0x"<<hex<<PC<<" (branchPC)"<<endl;
	}
	else {
		PC += 4;
		cout<<"New PC = 0x"<<hex<<PC<<" (PC + 4)"<<endl;
	}
	cout<<endl;
}

//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void Core::decode() {
	cout<<endl<<"!--------- DECODE ---------!"<<endl<<endl;
	unsigned int temp_instruction_word = if_of->instruction_word->Read();
	unsigned int temp_PC = if_of->PC->Read();

	cout<<"Exceuting Instruction 0x"<<hex<<temp_instruction_word<<" with PC 0x"<<temp_PC<<endl;

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

//executes the ALU operation based on ALUop
void Core::execute() {
	cout<<endl<<"!--------- EXECUTE ---------!"<<endl<<endl;
	//////////   Branch Unit  ///////////
	cout<<"*** Branch Unit"<<endl;
	if (isRet){
		branchPC = operand1;
		cout<<"branchPC: 0x"<<hex<<branchPC<<" (operand1)"<<endl;
	}
	else{
		branchPC = branchTarget;
		cout<<"branchPC: 0x"<<hex<<branchPC<<" (branchTarget)"<<endl;
	}
	

	if (isUbranch){
		isBranchTaken = true;
		cout<<"isBranchTaken: True"<<endl;
	}
	else if(isBeq && eq) {
		isBranchTaken = true;
		cout<<"isBranchTaken: True"<<endl;
	}
	else if(isBgt && gt){
		isBranchTaken = true;
		cout<<"isBranchTaken: True"<<endl;
	}
	else{
		isBranchTaken = false;
		cout<<"isBranchTaken: False"<<endl;
	}


	//////////   ALU  ///////////

	cout<<endl<<"*** ALU"<<endl;

	unsigned int A;
	unsigned int B;
	
	A = operand1;
	cout<<"A: "<<dec<<A<<endl;

	if (isImmediate){
		B = immx;
		cout<<"B: "<<dec<<B<<" (immx)"<<endl;
	}
	else {
		B = operand2;
		cout<<"B: "<<dec<<B<<" (operand2)"<<endl;
	}
	


	if (isAdd){
		cout<<"ADD operation"<<endl;
		aluResult = A + B;
		
	}
	if (isSub){		
		cout<<"SUB operation"<<endl;
		aluResult = A + (~B) + 1;
	}
	if (isCmp){
		cout<<"CMP operation"<<endl;
		if (A +(~B)+1  == 0){
			eq = true;
			gt = false;
			cout<<"Equal"<<endl;
		}
		else if (inst_bitset(A+(~B)+1,32,32)   == 0){
			gt = true;
			eq = false;
			cout<<"Greator than"<<endl;
		}
		else {
			eq = false;
			gt = false;
		}
		
	}

	if (isMul){
		cout<<"MUL operation"<<endl;
		aluResult = A * B;
	}

	if (isDiv){
		cout<<"DIV operation"<<endl;
		aluResult = A / B;
	}

	if (isMod){
		cout<<"MOD operation"<<endl;
		aluResult = A % B;
	}

	if (isLsl){
		cout<<"LSL operation"<<endl;
		aluResult = A << B;
	}

	if (isLsr){
		cout<<"LSR operation"<<endl;
		aluResult = A >> B;
	}

	if (isAsr){
		cout<<"ASR operation"<<endl;
		aluResult = A;
		unsigned int count = B;
		while(count){
			if (aluResult>>31 == 1){
				aluResult>>1;
				aluResult = aluResult | 0x80000000;
			}
			else{
				aluResult>>1;
			}		

			count--;
		}
	}

	if (isOr){
		cout<<"OR operation"<<endl;
		aluResult = A | B;
	}

	if (isNot){
		cout<<"NOT operation"<<endl;
		aluResult = ~B;
	}

	if (isAnd){
		cout<<"AND operation"<<endl;
		aluResult = A & B;
	}

	if (isMov){
		cout<<"MOV operation"<<endl;
		aluResult = B;
	}

	cout<<"aluResult: "<<dec<<aluResult<<" (0x"<<hex<<aluResult<<")"<<endl;




}

//perform the memory operation
void Core::mem_access() {
	cout<<endl<<"!--------- MEMORY ACCESS ---------!"<<endl<<endl;
	unsigned int mar = mem_address(aluResult);
	unsigned int mdr = operand2;

	if (isLd){
		cout<<"Reading from Memory at address 0x"<<hex<<aluResult<<endl;
		ldResult = MEM->Read(mar);
	}
	else if (isSt){
		cout<<"Writing to Memory at address 0x"<<hex<<aluResult<<" with data "<<dec<<mdr<<endl;
		MEM->Write(mar,mdr);
	}
	else {
		cout<<"Memory unit Disabled"<<endl;
	}
}
//writes the results back to register file
void Core::write_back() {
	cout<<endl<<"!--------- WRITE BACK ---------!"<<endl<<endl;
	unsigned int result;
	unsigned int addr;

	if (isWb){

		if (isLd){
			result = ldResult;
			cout<<"Writing data "<<dec<<result<<" (ldResult)";
		}
		else if (isCall){
			result = PC + 4;
			cout<<"Writing data "<<dec<<result<<" (PC + 4)";
		}
		else {
			result = aluResult;
			cout<<"Writing data "<<dec<<result<<" (aluResult)";
		}

		if (isCall){			
			addr = 15;
		}
		else {
			addr = inst_bitset(instruction_word,23,26);
		}
	
		cout<<" to register R"<<dec<<addr<<endl;
		R[addr] = result;

	}
	else {
		cout<<"Write Back Disabled"<<endl;
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

