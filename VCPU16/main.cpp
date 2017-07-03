#include "stdafx.h"

#include <stdio.h>
#include "VCPU16.h"
#include <chrono>
# include <thread>
void singleCPU(int ID)
{
	VCPU16 c = VCPU16();
	for (unsigned short i = 0; i < 25; i++)
		c.PUT(i, 0);

	// Load test code in to memory.
	c.PUT(0, 0b0000000000000000); // R[0] = 0
	c.PUT(1, 0b0000000100000000); // R[1] = 0
	c.PUT(2, 0b0000001000000000); // R[2] = 0
	c.PUT(3, 0b0000001100000000); // R[3] = 0
	c.PUT(4, 0b0000010000000000); // R[4] = 0
	c.PUT(5, 0b0000010100000000); // R[5] = 0
	c.PUT(6, 0b0000011000000000); // R[6] = 0
	c.PUT(7, 0b0000011100000000); // R[7] = 0

	c.PUT(8, 0b0000000001111111); // R[0] = 127
	c.PUT(9, 0b0000101100000000); // R[3] = R[3] + R[0]
	c.PUT(10, 0b0001001110000000); // R[3] = R[3] + 128
	c.PUT(11, 0b0101011100000000); // R[7] = !R[7]
	c.PUT(12, 0b0111011111110011); // JMP -13


	// Run the test code.
	long long itr = 1e8;

	auto start = std::chrono::high_resolution_clock::now();
	for (long long i = 0; i < itr; i++)
		c.doStep();
	auto finish = std::chrono::high_resolution_clock::now();

	long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
	float MHZ = (1e9 * itr / ns) / 1e6;
	std::cout << "Thread: " << ID << "SPEED: " << MHZ << "MHz\n";
}


int main()
{
	/*
	printf("CPU After INIT!\n");
	VCPU16 c = VCPU16();
	c.CORE_DUMP();
	c.MEM_DUMP(0, 25);
	for (unsigned short i = 0; i < 25; i++)
		c.PUT(i, 0);
	printf("\n");

	// Load test code in to memory.
	c.PUT(0, 0b0000000000000000); // R[0] = 0
	c.PUT(1, 0b0000000100000000); // R[1] = 0
	c.PUT(2, 0b0000001000000000); // R[2] = 0
	c.PUT(3, 0b0000001100000000); // R[3] = 0
	c.PUT(4, 0b0000010000000000); // R[4] = 0
	c.PUT(5, 0b0000010100000000); // R[5] = 0
	c.PUT(6, 0b0000011000000000); // R[6] = 0
	c.PUT(7, 0b0000011100000000); // R[7] = 0

	c.PUT(8, 0b0000000001111111); // R[0] = 127
	c.PUT(9, 0b0000101100000000); // R[3] = R[3] + R[0]
	c.PUT(10, 0b0001001110000000); // R[3] = R[3] + 128
	c.PUT(11, 0b0101011100000000); // R[7] = !R[7]
	c.PUT(12, 0b0111001100000000); // JMP R[6]

	c.MEM_DUMP(0, 15);

	// Run the test code.
	auto start = std::chrono::high_resolution_clock::now();
	long long itr = 1e10;
	for (long long i = 0; i < itr; i++)
		c.doStep();
	auto finish = std::chrono::high_resolution_clock::now();
	long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
	float MHZ = (1e9 * itr/ns) / 1e6;
	std::cout << MHZ << "MHz\n";

	/*
	//c.CORE_DUMP();

	c.doStep();
	//c.CORE_DUMP();

	c.doStep();
	//c.CORE_DUMP();

	c.doStep();
	//c.CORE_DUMP();
	*/

	// MULTI THREAD RUN
	static const int t = 1;
	std::thread threads[t];
	for (int i = 0; i < t; i++)
	{
		threads[i] = std::thread(singleCPU, i);
	}

	std::cout << "Launched from the main\n";

	for (int i = 0; i < t; i++)
	{
		threads[i].join();
	}
	return 0;
}
