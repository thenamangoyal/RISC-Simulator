all:
	g++ main.cpp Core.cpp Memory.cpp -o a

clean:
	rm *.o a