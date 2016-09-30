#include "Memory.h"

using namespace std;

Memory::Memory(int Size)
{
    capacity = Size;
    start_Add = new char[capacity];
}

int Memory::Read(int address) const
{
    if(address >= 0 && address < capacity)
    {
        int *add = (int *)(start_Add + address);
        return *add;
    }
}

void Memory::Write(int address, int data)
{
    if(address >= 0 && address < capacity)
    {
        int *add = (int *)(start_Add + address);
        *add = data;
    }
}

Memory::~Memory()
{
    //dtor
}
