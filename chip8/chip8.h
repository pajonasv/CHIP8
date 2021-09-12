#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <random>
class chip8
{
public:
	//current opcode
	unsigned short currentOpcode; //2 bytes

	//4K memory
	unsigned char memory[4096]; //1 byte each

	//CPU registers
	unsigned char V[16]; //1 byte each

	//index register
	unsigned short ir; //2 bytes

	//program counter
	unsigned short pc; //2 bytes

	//graphics
	unsigned char gfx[64 * 32]; //1 byte each

	//timers
	unsigned char delay_timer; //1 byte
	unsigned char sound_timer; //1 byte

	//stack
	unsigned short stack[16]; //2 bytes each
	//stack pointer
	unsigned short sp; //2 bytes

	//keypad
	unsigned char key[16]; //1 byte each


	
	bool drawFlag;
	bool clearScreenFlag;

	bool debugMode;
	chip8();
	void initialize();
	bool loadProgram(char * filename);
	void emulateCycle();

};

