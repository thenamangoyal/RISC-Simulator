==========================================
SimpleRISC Functional & Pipeline Simulator
==========================================

The project simulates SimpleRISC functional & pipeline processor using C++.


To compile and run the code in linux follow the follwing steps in the home directory of project folder.

*To compile execute the following command in terminal
make

*To run the functional program
./sim test/<INPUT MEM FILE>

*To run the pipeline program
./sim test/<INPUT MEM FILE> 1

*To export the result for a test file eg. array_sum.mem with pipeline
./sim test/array_sum.mem 1 >output

*To clean compiled code
make clean

The code generates a DATA_OUT.mem i.e. the complete memory at the termination of program 
and a STATE_OUT.mem i.e. complete state of processor at the termination of program.

The output contains instruction by instruction work done in each cycle in all stages namely -

1. FETCH
2. DEOCDE
3. EXECUTE
4. MEMORY ACCESS
5. WRITEBACK

Developed by
NAMAN GOYAL, 2015CSB1021
EESHAAN SHARMA, 2015CSB1011
