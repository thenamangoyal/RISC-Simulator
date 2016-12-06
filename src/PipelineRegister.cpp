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

#include "PipelineRegister.h"

PipelineRegister::PipelineRegister(bool pipe): bubble(pipe), PC(pipe), instruction_word(pipe), branchTarget(pipe), A(pipe), B(pipe), operand2(pipe), aluResult(pipe), ldResult(pipe), 	isSt(pipe), isLd(pipe), isBeq(pipe), isBgt(pipe), isRet(pipe), isImmediate(pipe), isWb(pipe), isUbranch(pipe), isCall(pipe), isAdd(pipe), isSub(pipe), isCmp(pipe), isMul(pipe), isDiv(pipe), isMod(pipe), isLsl(pipe), isLsr(pipe), isAsr(pipe), isOr(pipe), isAnd(pipe), isNot(pipe), isMov(pipe) {}

void PipelineRegister::WriteBubble(bool to_bubble) {

	if (to_bubble) {
		bubble.Write(true);

		PC.Write(0x0);
		instruction_word.Write(0x68000000);

		branchTarget.Write(0x0);
		A.Write(0x0);
		B.Write(0x0);
		operand2.Write(0x0);
		aluResult.Write(0x0);
		ldResult.Write(0x0);

		isSt.Write(false);
		isLd.Write(false);
		isBeq.Write(false);
		isBgt.Write(false);
		isRet.Write(false);
		isImmediate.Write(false);
		isWb.Write(false);
		isUbranch.Write(false);
		isCall.Write(false);
		isAdd.Write(false);
		isSub.Write(false);
		isCmp.Write(false);
		isMul.Write(false);
		isDiv.Write(false);
		isMod.Write(false);
		isLsl.Write(false);
		isLsr.Write(false);
		isAsr.Write(false);
		isOr.Write(false);
		isAnd.Write(false);
		isNot.Write(false);
		isMov.Write(false);
	}
	else {
		bubble.Write(false);
	}
}


void PipelineRegister::ForwardBubble(bool to_bubble) {
	
	bubble.Write(to_bubble);

}

void PipelineRegister::clock(){
	bubble.clock();
	PC.clock();
	instruction_word.clock();
	branchTarget.clock();
	A.clock();
	B.clock();
	operand2.clock();
	aluResult.clock();
	ldResult.clock();
	isSt.clock();
	isLd.clock();
	isBeq.clock();
	isBgt.clock();
	isRet.clock();
	isImmediate.clock();
	isWb.clock();
	isUbranch.clock();
	isCall.clock();
	isAdd.clock();
	isSub.clock();
	isCmp.clock();
	isMul.clock();
	isDiv.clock();
	isMod.clock();
	isLsl.clock();
	isLsr.clock();
	isAsr.clock();
	isOr.clock();
	isAnd.clock();
	isNot.clock();
	isMov.clock();
}