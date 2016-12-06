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

#include "Memory.h"

using namespace std;

Memory::Memory(unsigned int Size)
{
    capacity = Size;
    start_Add = new char[capacity];
}

unsigned int Memory::Read(unsigned int address) const
{
    if(address >= 0 && address <= capacity - sizeof(unsigned int))
    {
        unsigned int *add = (unsigned int *)(start_Add + address);
        return *add;
    }
}

void Memory::Write(unsigned int address, unsigned int data)
{
    if(address >= 0 && address <= capacity - sizeof(unsigned int))
    {
        unsigned int *add = (unsigned int *)(start_Add + address);
        *add = data;
    }
}

Memory::~Memory()
{
    delete [] start_Add;
}
