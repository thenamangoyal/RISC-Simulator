#ifndef MEMORY_H
#define MEMORY_H

#include <iostream>

class Memory
{
    public:
        Memory(unsigned int Size = 10000);
        unsigned int Read(unsigned int address) const;
        void Write(unsigned int address, unsigned int data);
        virtual ~Memory();

    protected:
    private:
        unsigned int capacity;
        char *start_Add;
};

#endif // MEMORY_H
