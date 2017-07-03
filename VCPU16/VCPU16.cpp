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

#if USE_OPFUNC_TABLE
	OPFUNC[0] = &VCPU16::NOP;
	OPFUNC[1] = &VCPU16::ADD;
	OPFUNC[2] = &VCPU16::ADI;
	OPFUNC[3] = &VCPU16::SUB;
	OPFUNC[4] = &VCPU16::MUL;
	OPFUNC[5] = &VCPU16::DIV;
	OPFUNC[6] = &VCPU16::RESERV;
	OPFUNC[7] = &VCPU16::RESERV;
	OPFUNC[8] = &VCPU16::RESERV;
	OPFUNC[9] = &VCPU16::RESERV;

	OPFUNC[10] = &VCPU16::NOT;
	OPFUNC[11] = &VCPU16::AND;
	OPFUNC[12] = &VCPU16::OR;
	OPFUNC[13] = &VCPU16::XOR;
	OPFUNC[14] = &VCPU16::JMP;
	OPFUNC[15] = &VCPU16::JX;
	OPFUNC[16] = &VCPU16::RCL;
	OPFUNC[17] = &VCPU16::RCR;
	OPFUNC[18] = &VCPU16::ST;
	OPFUNC[19] = &VCPU16::LD;

	OPFUNC[20] = &VCPU16::SET;
	OPFUNC[21] = &VCPU16::RESERV;
	OPFUNC[22] = &VCPU16::RESERV;
	OPFUNC[23] = &VCPU16::RESERV;
	OPFUNC[24] = &VCPU16::RESERV;
	OPFUNC[25] = &VCPU16::RESERV;
	OPFUNC[26] = &VCPU16::RESERV;
	OPFUNC[27] = &VCPU16::RESERV;
	OPFUNC[28] = &VCPU16::RESERV;
	OPFUNC[29] = &VCPU16::RESERV;

	OPFUNC[30] = &VCPU16::RESERV;
	OPFUNC[31] = &VCPU16::TRAP;
#endif
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
	if (PC == 0xFFFF)
	{
#if VERBOSE_CPU
		std::cout << "CPU REACHED END\n";
#endif
		return;
	}
	IR = memory[PC]; 
	PC++;
	// STEP 2: DECODE
	unsigned short OPCODE = IR >> 11;
	// STEP 3: Execute/ADDR GENERATION
#if USE_OPFUNC_TABLE
	(*this.*OPFUNC[OPCODE])();
#else
	switch (OPCODE)
	{
	case 0:
		NOP();
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
	case 6:
		RESERV();
		break;
	case 7:
		RESERV();
		break;
	case 8:
		RESERV();
		break;
	case 9:
		RESERV();
		break;
	case 10:
		NOT();
		break;
	case 11:
		AND();
		break;
	case 12:
		OR();
		break;
	case 13:
		XOR();
		break;
	case 14:
		JMP();
		break;
	case 15:
		JX();
		break;
	case 16:
		RCL();
		break;
	case 17:
		RCR();
		break;
	case 18:
		ST();
		break;
	case 19:
		LD();
		break;
	case 20:
		SET();
		break;
	case 21:
		RESERV();
		break;
	case 22:
		RESERV();
		break;
	case 23:
		RESERV();
		break;
	case 24:
		RESERV();
		break;
	case 25:
		RESERV();
		break;
	case 26:
		RESERV();
		break;
	case 27:
		RESERV();
		break;
	case 28:
		RESERV();
		break;
	case 29:
		RESERV();
		break;
	case 30:
		RESERV();
		break;
	case 31:
		TRAP();
		break;
	}
#endif
	
}

void VCPU16::updateFLAGS(unsigned const char DR)
{
	bool n = (REGISTERS[DR] & 0x8000) > 0;
	bool z = (REGISTERS[DR] & 0xFFFF) == 0;

	FLAGS = 0;
	
	// Negative
	if (n)
	{
		FLAGS = FLAGS | 0x8000;
	}
	if(!n && !z) // Positive
	{
		FLAGS = FLAGS | 0x4000;
	}
	if (z)
	{
		FLAGS = FLAGS | 0x2000;
	}

}

#pragma region OPCODES FUNCTIONS
void VCPU16::SET()
{
#if VERBOSE_CPU
	std::cout << "SET: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	bool f = (IR & 0b000000010000000) > 0;
	
	if (f)
	{	
		unsigned char SR = (IR & 0x0007); // Already LSB no need to shift.
		REGISTERS[DR] = REGISTERS[SR];

#if VERBOSE_CPU
		std::cout << "R[" << (int)DR << "] <- R[" << (int)SR << "]" << std::endl;
#endif
	}
	else
	{
		unsigned short IMD = IR & 0x007F; // Already LSB no need to shift.
		REGISTERS[DR] = IMD;

#if VERBOSE_CPU
		std::cout << "R[" << (int)DR << "] <- (" << IMD << ")"<<std::endl;
#endif
	}
}

void VCPU16::ADD()
{
#if VERBOSE_CPU
	std::cout << "ADD: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	REGISTERS[DR] = REGISTERS[DR] + REGISTERS[SR];

	updateFLAGS(DR);

#if VERBOSE_CPU
	std::cout  << "R[" << (int)DR << "] <- R[" << (int)DR << "] + R[" << (int)SR <<"]" << std::endl;
#endif
}

void VCPU16::ADI()
{
#if VERBOSE_CPU
	std::cout << "ADI: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned short IMD = (IR & 0x00FF);
	REGISTERS[DR] = REGISTERS[DR] + IMD;

	updateFLAGS(DR);

#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] + (" << (int)IMD << ")" << std::endl;
#endif
}

void VCPU16::SUB()
{
#if VERBOSE_CPU
	std::cout << "SUB: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	REGISTERS[DR] = REGISTERS[DR] - REGISTERS[SR];

	updateFLAGS(DR);

#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] - R[" << (int)SR << "]" << std::endl;
#endif
}

void VCPU16::MUL()
{
#if VERBOSE_CPU
	std::cout << "MUL: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	REGISTERS[DR] = REGISTERS[DR] - REGISTERS[SR];

	updateFLAGS(DR);

#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] * R[" << (int)SR << "]" << std::endl;
#endif
}

void VCPU16::DIV()
{
#if VERBOSE_CPU
	std::cout << "DIV: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	if (REGISTERS[SR] != 0)
		REGISTERS[DR] = REGISTERS[DR] / REGISTERS[SR];
	else
		REGISTERS[DR] = 0;

	updateFLAGS(DR);

#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] / R[" << (int)SR << "]" << std::endl;
#endif
}

void VCPU16::NOT()
{
#if VERBOSE_CPU
	std::cout << "NOT: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	REGISTERS[DR] = ~REGISTERS[DR];

#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- !R[" << (int)DR << "]" << std::endl;
#endif
}

void VCPU16::AND()
{
#if VERBOSE_CPU
	std::cout << "AND: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	REGISTERS[DR] = REGISTERS[DR] & REGISTERS[SR];
#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] & R[" << (int)SR << "]" << std::endl;
#endif
}

void VCPU16::OR()
{
#if VERBOSE_CPU
	std::cout << "OR : ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	REGISTERS[DR] = REGISTERS[DR] | REGISTERS[SR];
	
#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] & R[" << (int)SR << "]" << std::endl;
#endif

}

void VCPU16::XOR()
{
#if VERBOSE_CPU
	std::cout << "XOR: ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	REGISTERS[DR] = REGISTERS[DR] ^ REGISTERS[SR];

#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)DR << "] & R[" << (int)SR << "]" << std::endl;
#endif
}

void VCPU16::JMP()
{

#if VERBOSE_CPU
	std::cout << "JMP: ";
#endif
	bool f = (IR & 0b0000010000000000) > 0;
	if (f)
	{
		unsigned short IMD = (IR & 0b0000001111111111) | ((IR & 0x200) ? 0xFC00 : 0);
		PC = PC + IMD;
#if VERBOSE_CPU
		std::cout << "PC <- PC + ("<< (short)IMD << ")" << std::endl;
#endif
	}
	else
	{
		unsigned char SR = (IR & 0b0000001110000000) >> 7;
		PC = REGISTERS[SR];
#if VERBOSE_CPU
		std::cout << "PC <- R[" << (int)SR << "]" << std::endl;
#endif
	}
}

void VCPU16::JX()
{
	bool n = (FLAGS & 0x4000) > 0;
	bool p = (FLAGS & 0x4000) > 0;
	bool z = (FLAGS & 0x2000) > 0;
#if VERBOSE_CPU
	std::cout << "J";
	if (n)
		std::cout << "N";
	if(p)
		std::cout << "P";
	if (z)
		std::cout << "Z ";
	std::cout << ": ";
#endif
	if (n || p || z)
	{
		unsigned short IMD = (IR & 0b0000000011111111) | ((IR & 0x80) ? 0xFF00 : 0);
		PC = PC + IMD;
#if VERBOSE_CPU
		std::cout << "PC <- PC + (" << (short)IMD << ")" << std::endl;
#endif
	}
#if VERBOSE_CPU
	else
	{
		std::cout << "PC <- PC" << std::endl;
	}
#endif
}

void VCPU16::RCL()
{
#if VERBOSE_CPU
	std::cout << "RCL : ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	unsigned char IMD = (IR & 0x001F);
	REGISTERS[DR] = REGISTERS[DR] << IMD;
#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)SR << "] << (" << (int)IMD << ")" << std::endl;
#endif
}

void VCPU16::RCR()
{
#if VERBOSE_CPU
	std::cout << "RCR : ";
#endif
	unsigned char DR = (IR & 0x0700) >> 8;
	unsigned char SR = (IR & 0x00E0) >> 5;
	unsigned char IMD = (IR & 0x001F);
	REGISTERS[DR] = REGISTERS[DR] >> IMD;
#if VERBOSE_CPU
	std::cout << "R[" << (int)DR << "] <- R[" << (int)SR << "] >> (" << (int)IMD << ")" << std::endl;
#endif
}

void VCPU16::ST()
{
#if VERBOSE_CPU
	std::cout << "ST  : ";
#endif
	bool f = (IR & 0b0000010000000000) > 0;
	unsigned char SR = (IR & 0b00000001110000000) >> 7;
	if (f)
	{
		unsigned char ADDR = (IR & 0b00000000001110000) >> 4;
		memory[REGISTERS[ADDR]] = REGISTERS[SR];
#if VERBOSE_CPU
		std::cout << "[" << REGISTERS[ADDR] << "] <- R[" << (int)SR << "]" << std::endl;
#endif
	}
	else
	{
		unsigned short IMD = (IR & 0b0000000001111111) | ((IR & 0x40) ? 0xFF80 : 0);
		memory[PC + IMD] = REGISTERS[SR];
#if VERBOSE_CPU
		std::cout << "[" << PC + IMD << "] <- R[" << (int)SR << "]" << std::endl;
#endif
	}
}

void VCPU16::LD()
{
#if VERBOSE_CPU
	std::cout << "LD  : ";
#endif
	bool f = (IR & 0b0000010000000000) > 0;
	unsigned char SR = (IR & 0b00000001110000000) >> 7;
	if (f)
	{
		unsigned char ADDR = (IR & 0b00000000001110000) >> 4;
		REGISTERS[SR] = memory[REGISTERS[ADDR]];
#if VERBOSE_CPU
		std::cout << "R[" << (int)SR << "] <- [" << REGISTERS[ADDR] << "]" << std::endl;
#endif
	}
	else
	{
		unsigned short IMD = (IR & 0b0000000001111111) | ((IR & 0x40) ? 0xFF80 : 0);
		REGISTERS[SR] = memory[PC + IMD];
#if VERBOSE_CPU
		std::cout << "R[" << (int)SR << "] <- [" << PC + IMD << "]" << std::endl;
#endif
	}
}

void VCPU16::TRAP()
{
#if VERBOSE_CPU
	std::cout << "TRAP: ";
#endif

	*REG_H = PC;
	unsigned short trapV = IR & 0x00FF;
	PC = trapV;

#if VERBOSE_CPU
	std::cout << "R[7] <- PC (" << PC << ") | PC <- (" << trapV << ")" << std::endl;
#endif
}


void VCPU16::NOP()
{
#if VERBOSE_CPU
	std::cout << "NOP *" << std::endl;
#endif
	return;
}

void VCPU16::RESERV()
{
#if VERBOSE_CPU
	std::cout << "RSRV*" << std::endl;
#endif
	return;
}
#pragma endregion
