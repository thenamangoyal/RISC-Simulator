#ifndef MEMORY_H
#define MEMORY_H

#include<iostream>

class Memory
{
    public:
        Memory(int Size = 10000);
        int Read(int) const;
        void Write(int, int);
        virtual ~Memory();

    protected:
    private:
        int capacity;
        char *start_Add;
};

#endif // MEMORY_H
