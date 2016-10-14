#include "Core.h"

using namespace std;


Core::Core(){
	INST_MAX = 0;
	MEM = new Memory(MEM_CAPACITY);
}

Core::~Core(){
	delete MEM;
}

void Core::run_simplesim(){
	int counter = 0;
	while (PC != INST_MAX){
		cout<<"========================"<<endl;
		cout<<"INSTRUNCTION "<<dec<<counter+1<<endl;
		cout<<"========================"<<endl;
		fetch_begin();
		decode();
		execute();
		mem_access();
		write_back();
		fetch_end();
		counter++;
	}
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
	eq = false;
	gt = false;
}

//reads from the instruction memory
void Core::fetch_begin() {
	cout<<endl<<"!--------- FETCH ---------!"<<endl<<endl;
	instruction_word = MEM->Read(PC);	
	cout<<"Instruction 0x"<<hex<<instruction_word<<" read at address 0x"<<hex<<PC<<endl;
	//cout<<bitset<32> (instruction_word)<<" : Instruction encoding"<<endl;
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
	unsigned int opcode1 = inst_bitset(instruction_word, 28, 28);
	unsigned int opcode2 = inst_bitset(instruction_word, 29, 29);
	unsigned int opcode3 = inst_bitset(instruction_word, 30, 30);
	unsigned int opcode4 = inst_bitset(instruction_word, 31, 31);
	unsigned int opcode5 = inst_bitset(instruction_word, 32, 32);
	unsigned int I_bit = inst_bitset(instruction_word, 27, 27);
	cout<<"Control Signals"<<endl;
	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1 && opcode1 == 1){
		isSt = true;
		cout<<"isSt ";
	}
	else{
		isSt = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1 && opcode1 == 0){
		isLd = true;
		cout<<"isLd ";
	}
	else{
		isLd = false;
	}
	
	if(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 0){
		isBeq = true;
		cout<<"isBeq ";
	}
	else{
		isBeq = false;
	}

	if(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 1){
		isBgt = true;
		cout<<"isBgt ";
	}
	else{
		isBgt = false;
	}

	if(opcode5 == 1 && opcode4 == 0 && opcode3 == 1 && opcode2 == 0 && opcode1 == 0){
		isRet = true;
		cout<<"isRet ";
	}
	else{
		isRet = false;
	}

	if(I_bit == 1){
		isImmediate = true;
		cout<<"isImmediate ";
	}
	else{
		isImmediate = false;
	}

	if( !(opcode5 == 1 || ( opcode5 == 0 && opcode3 == 1 && opcode1 == 1 && ( opcode4 == 1 || opcode2 == 0) ) ) 	|| 	(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1)  ){
		isWb = true;
		cout<<"isWb ";
	}
	else{
		isWb = false;
	}

	if( opcode5 == 1 && opcode4 == 0 && (	(opcode3 == 0 && opcode2 == 1) || (opcode3 == 1 && opcode2 == 0 && opcode1 == 0) ) 	){
		isUbranch = true;
		cout<<"isUbranch ";
	}
	else{
		isUbranch = false;
	}

	if(opcode5 == 1 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1){
		isCall = true;
		cout<<"isCall ";
	}
	else{
		isCall = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 0){
		isAdd = true;
		cout<<"isAdd ";
	}
	else if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 1){
		isAdd = true;
		cout<<"isAdd ";
	}
	else{
		isAdd = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 0 && opcode1 == 1){
		isSub = true;
		cout<<"isSub ";
	}
	else{
		isSub = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 0 && opcode1 == 1){
		isCmp = true;
		cout<<"isCmp ";
	}
	else{
		isCmp = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 0){
		isMul = true;
		cout<<"isMul ";
	}
	else{
		isMul = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1){
		isDiv = true;
		cout<<"isDiv ";
	}
	else{
		isDiv = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 0 && opcode1 == 0){
		isMod = true;
		cout<<"isMod ";
	}
	else{
		isMod = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 1 && opcode1 == 0){
		isLsl = true;
		cout<<"isLsl ";
	}
	else{
		isLsl = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 1 && opcode1 == 1){
		isLsr = true;
		cout<<"islsr ";
	}
	else{
		isLsr = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 1 && opcode2 == 0 && opcode1 == 0){
		isAsr = true;
		cout<<"isAsr ";
	}
	else{
		isAsr = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 1 && opcode1 == 1){
		isOr = true;
		cout<<"isOr ";
	}
	else{
		isOr = false;
	}

	if(opcode5 == 0 && opcode4 == 0 && opcode3 == 1 && opcode2 == 1 && opcode1 == 0){
		isAnd = true;
		cout<<"isAnd ";
	}
	else{
		isAnd = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 0 && opcode1 == 0){
		isNot = true;
		cout<<"isNot ";
	}
	else{
		isNot = false;
	}

	if(opcode5 == 0 && opcode4 == 1 && opcode3 == 0 && opcode2 == 0 && opcode1 == 1){
		isMov = true;
		cout<<"isMov ";
	}
	else{
		isMov = false;
	}
	cout<<endl;


	//////////   immx calculation  ///////////
	cout<<endl;
	unsigned int imm = inst_bitset(instruction_word, 1, 16);
	unsigned int u = inst_bitset(instruction_word, 17, 17);
	unsigned int h = inst_bitset(instruction_word, 18, 18);


	if (u == 0 && h == 0){
		if (inst_bitset(instruction_word, 16, 16) == 1){
			immx = 0xffff0000 | imm;
		}
		else{
			immx = imm;
		}
		cout<<"Immediate is "<<dec<<immx<<" (0x"<<hex<<immx<<")"<<endl;
	}
	else if (u == 1){
		immx = imm;
		cout<<"Immediate is "<<dec<<immx<<" (0x"<<hex<<immx<<") and is Unsigned"<<endl;
	}
	else{
		immx = imm<<16;
		cout<<"Immediate is "<<dec<<immx<<" (0x"<<hex<<immx<<") ans is High"<<endl;
	}


	//////////   branchTarget calculation  ///////////
	unsigned int offset = inst_bitset(instruction_word, 1,27);

	if (inst_bitset(instruction_word, 27, 27) == 1){
		branchTarget = 0xe0000000 | (offset<<2);
	}
	else {
		branchTarget = offset<<2;
	}
	
	branchTarget += PC;
	cout<<endl<<"branchTarget is 0x"<<hex<<branchTarget<<endl;


	//////////   Reading Register File  ///////////
	unsigned int rd = inst_bitset(instruction_word, 23,26);
	unsigned int rs1 = inst_bitset(instruction_word, 19,22);
	unsigned int rs2 = inst_bitset(instruction_word, 15,18);

	cout<<endl<<"rd: R"<<dec<<rd<<", rs1: R"<<dec<<rs1<<", rs2: R"<<dec<<rs2<<endl;

	if (isRet){
		operand1 = R[15];
		cout<<"Operand1: "<<dec<<operand1<<" (Read from ra OR R15)"<<endl;
	}
	else{
		operand1 = R[rs1];
		cout<<"Operand1: "<<dec<<operand1<<" (Read from rs1)"<<endl;
	}
	

	if (isSt){
		operand2 = R[rd];
		cout<<"Operand2: "<<dec<<operand2<<" (Read from rd)"<<endl;
	}
	else{
		operand2 = R[rs2];
		cout<<"Operand2: "<<dec<<operand2<<" (Read from rs2)"<<endl;
	}

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

