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

#ifndef PIPELINE_REGISTER_H
#define PIPELINE_REGISTER_H

#include "Register.h"

class PipelineRegister {
public:
	Register<bool> bubble;

	Register<unsigned int> PC;
	Register<unsigned int> instruction_word;
	Register<unsigned int> branchTarget;
	Register<unsigned int> A;
	Register<unsigned int> B;
	Register<unsigned int> operand2;
	Register<unsigned int> aluResult;
	Register<unsigned int> ldResult;

	Register<bool> isSt;
	Register<bool> isLd;
	Register<bool> isBeq;
	Register<bool> isBgt;
	Register<bool> isRet;
	Register<bool> isImmediate;
	Register<bool> isWb;
	Register<bool> isUbranch;
	Register<bool> isCall;
	Register<bool> isAdd;
	Register<bool> isSub;
	Register<bool> isCmp;
	Register<bool> isMul;
	Register<bool> isDiv;
	Register<bool> isMod;
	Register<bool> isLsl;
	Register<bool> isLsr;
	Register<bool> isAsr;
	Register<bool> isOr;
	Register<bool> isAnd;
	Register<bool> isNot;
	Register<bool> isMov;
public:
	PipelineRegister(bool pipe);
	void clock();
	void WriteBubble(bool to_bubble);
	void ForwardBubble(bool to_bubble);
};

#endif