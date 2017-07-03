#pragma once

#include<iostream>
#include<bitset>

#define VERBOSE_CPU 0
#define USE_OPFUNC_TABLE 0
class VCPU16;
typedef void (VCPU16::* FP)(void);

class VCPU16
{
public:
	VCPU16(int maxMemorySize = 65565);
	~VCPU16();

	// Functions
	void CORE_DUMP();

	void MEM_DUMP(unsigned short start_MA, unsigned short end_MA);

	void RESET();

	void doStep();

	void PUT(unsigned short MA, unsigned short MD);
private:
	unsigned short* memory;
	bool* memoryValid;

	unsigned short IR;
	unsigned short PC;
	unsigned short FLAGS;

	// General purpose registers.
	unsigned short * REGISTERS;
	unsigned short * REG_A;
	unsigned short * REG_B;
	unsigned short * REG_C;
	unsigned short * REG_D;
	unsigned short * REG_E;
	unsigned short * REG_F;
	unsigned short * REG_G;
	unsigned short * REG_H;

#if USE_OPFUNC_TABLE
	// OPCODE FUNCTION TABLE
	FP OPFUNC[32];
#endif

	// Memory control
	unsigned short MAR;
	unsigned short * MDR;
	bool memread;
	bool memwrite;

	// Functions
	void init();

	void updateFLAGS(unsigned const char DR);

	// OPCODE FUNCTIONS
	void SET();

	void ADD();

	void ADI();

	void SUB();

	void MUL();

	void DIV();

	void NOT();

	void AND();
	
	void OR();

	void XOR();

	void JMP();

	void JX();

	void RCL();

	void RCR();

	void ST();

	void LD();

	void TRAP();

	void NOP();

	void RESERV();
};

