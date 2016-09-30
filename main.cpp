#include <iostream>

using namespace std;

#include "Memory.h"

int main()
{
    Memory mem_Block(12);
    mem_Block.Write(0,5);
    mem_Block.Write(,10);
    cout<<mem_Block.Read(0)<<endl;
    return 0;
}
