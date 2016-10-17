===============================
SimpleRISC Functional Simulator
===============================

The project simulates SimpleRISC functional processor using C++.


To compile and run the code in linux follow the follwing steps.

*To compile execute the following command in terminal
make

*To run the program
./sim TestCases/<INPUT MEM FILE>

*To export the result for a test file eg. array_sum.mem
./sim TestCases/array_sum.mem>output


The code generates a DATA_OUT.mem i.e. the complete memory at the termination of program.

The output contains instruction by instruction work done in each cycle in all stages namely -

1. FETCH
2. DEOCDE
3. EXECUTE
4. MEMORY ACCESS
5. WRITEBACK

Developed by
NAMAN GOYAL, 2015CSB1021
EESHAAN SHARMA, 2015CSB1011

