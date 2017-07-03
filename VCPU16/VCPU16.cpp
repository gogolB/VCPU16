// VCPU16.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "VCPU16.h"


VCPU16::VCPU16(int maxMemory)
{
	memory = new unsigned short[maxMemory];
	memoryValid = new bool[maxMemory];
	REGISTERS = new unsigned short[8];

	REG_A = &REGISTERS[0];
	REG_B = &REGISTERS[1];
	REG_C = &REGISTERS[2];
	REG_D = &REGISTERS[3];
	REG_E = &REGISTERS[4];
	REG_F = &REGISTERS[5];
	REG_G = &REGISTERS[6];
	REG_H = &REGISTERS[7];

	init();
}


VCPU16::~VCPU16()
{
	delete [] memory;
	delete [] memoryValid;
}

void VCPU16::init()
{
	IR = 0;
	PC = 0;
	FLAGS = 0;
	// Init general registers
	REGISTERS[0] = 0; // REG_A
	REGISTERS[1] = 0; // REG_B
	REGISTERS[2] = 0; // REG_C
	REGISTERS[3] = 0; // REG_D
	REGISTERS[4] = 0; // REG_E
	REGISTERS[5] = 0; // REG_F
	REGISTERS[6] = 0; // REG_G
	REGISTERS[7] = 0; // REG_H
	// Memory Control
	MAR = 0;
	MDR = nullptr;
	memread = false;
	memwrite = false;
}

void VCPU16::RESET()
{
	init();
}

void VCPU16::CORE_DUMP()
{
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "|                   CORE	 DUMP                  |" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "IR   : " << std::bitset<16>(IR) << "\t(" << std::hex << IR << ")" << std::endl;
	std::cout << "PC   : " << std::bitset<16>(PC) << "\t(" << std::hex << PC << ")" << std::endl;
	std::cout << "FLAG : " << std::bitset<16>(FLAGS) << "\t(" << std::hex << FLAGS << ")" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "REG_A: " << std::bitset<16>(*REG_A) << "\t(" << std::hex << *REG_A << ")|[" << std::dec << *REG_A << "]" << std::endl;
	std::cout << "REG_B: " << std::bitset<16>(*REG_B) << "\t(" << std::hex << *REG_B << ")|[" << std::dec << *REG_B << "]" << std::endl;
	std::cout << "REG_C: " << std::bitset<16>(*REG_C) << "\t(" << std::hex << *REG_C << ")|[" << std::dec << *REG_C << "]" << std::endl;
	std::cout << "REG_D: " << std::bitset<16>(*REG_D) << "\t(" << std::hex << *REG_D << ")|[" << std::dec << *REG_D << "]" << std::endl;
	std::cout << "REG_E: " << std::bitset<16>(*REG_E) << "\t(" << std::hex << *REG_E << ")|[" << std::dec << *REG_E << "]" << std::endl;
	std::cout << "REG_F: " << std::bitset<16>(*REG_F) << "\t(" << std::hex << *REG_F << ")|[" << std::dec << *REG_F << "]" << std::endl;
	std::cout << "REG_G: " << std::bitset<16>(*REG_G) << "\t(" << std::hex << *REG_G << ")|[" << std::dec << *REG_G << "]" << std::endl;
	std::cout << "REG_H: " << std::bitset<16>(*REG_H) << "\t(" << std::hex << *REG_H << ")|[" << std::dec << *REG_H << "]" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "MAR  : " << std::bitset<16>(MAR) << "\t(" << std::hex << MAR << ")|[" << std::dec << MAR << "]" << std::endl;
	if(MDR!=nullptr)
		std::cout << "MDR  : " << std::bitset<16>(*MDR) << "\t(" << std::hex << *MDR << ")|[" << std::dec << *MDR << "]" << std::endl;
	std::cout << "MW   : " << std::bitset<1>(memwrite) << std::endl;
	std::cout << "MR   : " << std::bitset<1>(memread)  <<  std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
}

void VCPU16::MEM_DUMP(unsigned short start_MA, unsigned short end_MA)
{
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "|                   MEM DUMP                  |" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	unsigned short i = start_MA;
	for (; i < end_MA; i++)
	{
		std::cout << "|" << i << " (0x" << std::hex << i << ")|\t" << std::bitset<16>(memory[i]) << " (0x" << std::hex << memory[i] << ")|[" << std::dec << memory[i] << "]" << std::endl;
	}
	std::cout << "-----------------------------------------------" << std::endl;
}

void VCPU16::PUT(unsigned short MA, unsigned short MD)
{
	memory[MA] = MD;
}

void VCPU16::doStep()
{
	// STEP 1: FETCH
	IR = memory[PC]; 
	if (PC == 0xFFFF)
	{
#ifdef VERBOSE_CPU
		std::cout << "CPU REACHED END\n";
#endif
		return;
	}
		
	PC++;
	// STEP 2: DECODE
	unsigned short OPCODE = IR >> 11;
	// STEP 3: Execute/ADDR GENERATION
	switch (OPCODE)
	{
	case 0:
		SET();
		break;
	case 1:
		ADD();
		break;
	case 2:
		ADI();
		break;
	case 3:
		SUB();
		break;
	case 4:
		MUL();
		break;
	case 5:
		DIV();
		break;
	case 10:
		NOT();
		break;
	case 14:
		JMP();
		break;
	}
	/*
	// STEP 4: MEMORY ACCESS
	if (memread)
	{
		*MDR = memory[MAR];
		memread = false;
	}

	// STEP 5: STORE
	if (memwrite)
	{
		memory[MAR] = *MDR;
		memwrite = false;
	}
	*/
}

#pragma region OPCODES FUNCTIONS
void VCPU16::SET()
{
#ifdef VERBOSE_CPU
	std::cout << "SET: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	bool f = (IR & 0b000000010000000) > 0;
	
	if (f)
	{	
		unsigned char SR = (IR & 0x0007); // Already LSB no need to shift.
		REGISTERS[DR] = REGISTERS[SR];

#ifdef VERBOSE_CPU
		std::cout << "R[" << (int)DR << "] <- R[" << (int)SR << "]" << std::endl;
#endif
	}
	else
	{
		unsigned short IMD = IR & 0x007F; // Already LSB no need to shift.
		REGISTERS[DR] = IMD;

#ifdef VERBOSE_CPU
		std::cout << "R[" << (int)DR << "] <- (" << IMD << ")"<<std::endl;
#endif
	}
}

void VCPU16::ADD()
{
#ifdef VERBOSE_CPU
	std::cout << "ADD: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	REGISTERS[DR] = REGISTERS[DR] + REGISTERS[SR];

#ifdef VERBOSE_CPU
	std::cout  << "R[" << (int)DR << "] <- R[" << (int)DR << "] + R[" << (int)SR <<"]" << std::endl;
#endif
}

void VCPU16::ADI()
{
#ifdef VERBOSE_CPU
	std::cout << "ADI: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned short IMD = (IR & 0x00FF);
	REGISTERS[DR] = REGISTERS[DR] + IMD;
#ifdef VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] + (" << (int)IMD << ")" << std::endl;
#endif
}

void VCPU16::SUB()
{
#ifdef VERBOSE_CPU
	std::cout << "SUB: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	REGISTERS[DR] = REGISTERS[DR] - REGISTERS[SR];

#ifdef VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] - R[" << (int)SR << "]" << std::endl;
#endif
}

void VCPU16::MUL()
{
#ifdef VERBOSE_CPU
	std::cout << "MUL: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	REGISTERS[DR] = REGISTERS[DR] - REGISTERS[SR];

#ifdef VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] * R[" << (int)SR << "]" << std::endl;
#endif
}

void VCPU16::DIV()
{
#ifdef VERBOSE_CPU
	std::cout << "DIV: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	if (REGISTERS[SR] != 0)
		REGISTERS[DR] = REGISTERS[DR] / REGISTERS[SR];
	else
		REGISTERS[DR] = 0;

#ifdef VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] / R[" << (int)SR << "]" << std::endl;
#endif
}

void VCPU16::NOT()
{
#ifdef VERBOSE_CPU
	std::cout << "NOT: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	REGISTERS[DR] = ~REGISTERS[DR];

#ifdef VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- !R[" << (int)DR << "]" << std::endl;
#endif
}


void VCPU16::JMP()
{
#ifdef VERBOSE_CPU
	std::cout << "JMP: ";
#endif
	bool f = (IR & 0b0000010000000000) > 0;
	if (f)
	{
		unsigned short IMD = (IR & 0b0000001111111111);
		IMD = IMD | ((IR & 0x200) ? 0xFC00 : 0);
		PC = PC + IMD;
#ifdef VERBOSE_CPU
		std::cout << "PC <- PC + ("<< (short)IMD << ")" << std::endl;
#endif
	}
	else
	{
		unsigned char SR = (IR & 0b0000001110000000) >> 7;
		PC = REGISTERS[SR];
#ifdef VERBOSE_CPU
		std::cout << "PC <- R[" << (int)SR << "]" << std::endl;
#endif
	}
}

#pragma endregion
