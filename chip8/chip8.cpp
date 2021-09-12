#include "chip8.h"

chip8::chip8() {
	srand(time(NULL));
	drawFlag = false;
}


void chip8::initialize() {
	pc = 0x200;
	currentOpcode = 0;
	ir = 0;
	sp = 0;

	for (int i = 0; i < sizeof(V); i++) {
		V[i] = 0;
	
	}
	
	for (int i = 0; i < sizeof(gfx); i++) {
		gfx[i] = 0;

	}

	unsigned char chip8_fontset[80] =
	{
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	for (int i = 0; i < sizeof(chip8_fontset); i++) {
		memory[0x50 + i] = chip8_fontset[i];
	}
}


bool chip8::loadProgram(const char* filename) {
	std::ifstream progfile;
    progfile.open(filename,std::ifstream::binary);
	if (progfile.fail()) {
		return false;
	}
	char byte;
	int i = 0;
	while (progfile.read(&byte, 1)) {
		memory[i + 512] = byte;
		i++;
	}
	for (int i = 0; i < sizeof(memory); i++) {
		if (i % 10 == 0) {
			std::cout << std::dec <<std::endl << i << ": ";
		}

		std::cout << std::hex<< (int)memory[i] << " ";
		
	}
	std::cout << std::endl;
	

}
void chip8::emulateCycle() {
	// Fetch Opcode
	currentOpcode = memory[pc] << 8 | memory[pc + 1];
	// Decode Opcode

	std::cout << currentOpcode << std::endl;

	//OPCODE DESCRIPTIONS ARE FROM WIKIPEDIA
	//https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description
	switch (currentOpcode & 0xF000) {
	
	case 0x0000:
		switch (currentOpcode & 0x000F) {
		
		case 0x0000: // 0x00E0: Clears the screen        
			
			for (int i = 0; i < sizeof(gfx); i++) {
				gfx[i] = 0;
			}
			drawFlag = true;
			pc += 2;
			break;

		case 0x000E: // 0x00EE: Returns from subroutine          
			sp--;
			pc = stack[sp];
			pc+=2;
			
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", currentOpcode);
		}
		break;
	
	case 0x1000: //1NNN: jumps to address NNN
		pc = currentOpcode & 0x0FFF;
		break;
	
	case 0x2000: //2NNN: Calls subroutine at NNN.
		stack[sp] = pc;
		sp++;
		pc = currentOpcode & 0x0FFF;
		break;

	case 0x3000:  //3XNN: Skips the next instruction if VX equals NN.
		if (V[(currentOpcode & 0x0F00) >> 8] == (currentOpcode & 0x00FF)) {
			pc += 2;
		}
		pc += 2;
		break;

	case 0x4000: //4XNN: Skips the next instruction if VX does not equal NN.
		if (V[(currentOpcode & 0x0F00) >> 8] != (currentOpcode & 0x00FF)) {
			pc += 2;
		}
		pc += 2;
		break;

	case 0x5000: //5XY0: Skips the next instruction if VX equals VY.

		if (V[(currentOpcode & 0x0F00) >> 8] == V[(currentOpcode & 0x00F0) >> 4]) {
			pc += 2;
		}
		pc += 2;
		break;

	case 0x6000: //6XNN:	Sets VX to NN.
		V[(currentOpcode & 0x0F00) >> 8] = (currentOpcode & 0x00FF);
		pc += 2;
		break;

	case 0x7000: //7XNN: Adds NN to VX. (Carry flag is not changed);
		V[(currentOpcode & 0x0F00) >> 8] += (currentOpcode & 0x00FF);
		pc += 2;
		break;

	case 0x8000: 
		switch (currentOpcode & 0x000F) {
		
		case 0x0000: //8XY0:	Sets VX to the value of VY.
			V[(currentOpcode & 0x0F00) >> 8] = V[(currentOpcode & 0x00F0) >> 4];
			pc += 2;
			break;
		
		case 0x0001: //8XY1: Sets VX to VX or VY. (Bitwise OR operation);
			V[(currentOpcode & 0x0F00) >> 8] = V[(currentOpcode & 0x0F00) >> 8] | V[(currentOpcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0002: //8XY2: Sets VX to VX and VY. (Bitwise AND operation);
			V[(currentOpcode & 0x0F00) >> 8] = V[(currentOpcode & 0x0F00) >> 8] & V[(currentOpcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0003: //8XY3: 	Sets VX to VX xor VY.

			V[(currentOpcode & 0x0F00) >> 8] = V[(currentOpcode & 0x0F00) >> 8] ^ V[(currentOpcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0004: //8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
			if (V[(currentOpcode & 0x00F0) >> 4] > (0xFF - V[(currentOpcode & 0x0F00) >> 8])) {
				V[0xF] = 1; //carry
			}
			else {
				V[0xF] = 0;
			}
			V[(currentOpcode & 0x0F00) >> 8] += V[(currentOpcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0005: //8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
			if (V[(currentOpcode & 0x00F0) >> 4] > V[(currentOpcode & 0x0F00) >> 8]) {
				V[0xF] = 0; //borrow
			}
			else {
				V[0xF] = 1;
			}
			V[(currentOpcode & 0x0F00) >> 8] = V[(currentOpcode & 0x0F00) >> 8] - V[(currentOpcode & 0x00F0) >> 4];
			pc += 2;
			break;

		case 0x0006: //8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
			V[0xF] = V[(currentOpcode & 0x0F00) >> 8] & 0x1;
			V[(currentOpcode & 0x0F00) >> 8] >>= 1;
			pc += 2;
			break;

		case 0x0007: //8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.
			if (V[(currentOpcode & 0x00F0) >> 4] < (V[(currentOpcode & 0x0F00) >> 8])) {
				V[0xF] = 0; //borrow
			}
			else {
				V[0xF] = 1;
			}
			
			V[(currentOpcode & 0x0F00) >> 8] = V[(currentOpcode & 0x00F0) >> 4] - V[(currentOpcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x000E: //8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1
			V[0xF] = V[(currentOpcode & 0x0F00) >> 8] >> 7;
			V[(currentOpcode & 0x0F00) >> 8] <<= 1;
			pc += 2;
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", currentOpcode);
			break;
		}
		break;
	
	case 0x9000:  //9XY0 Skips the next instruction if VX does not equal VY. 
		if (V[(currentOpcode & 0x0F00) >> 8] != V[(currentOpcode & 0x00F0) >> 4]) {
			pc += 2;
		}
		pc += 2;
		break;

	case 0xA000: //ANNN: Sets I to the address NNN.
		ir = currentOpcode & 0x0FFF;
		pc += 2;
		break;
	
	case 0xB000: //BNNN: Jumps to the address NNN plus V0.

		pc = (currentOpcode & 0x0FFF) + V[0x0];
		break;

	case 0xC000: //CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
		
		V[(currentOpcode & 0x0F00) >> 8] = (rand() % 0xFF) & (currentOpcode & 0x00FF);
		pc += 2;
		break;
	
	case 0xD000: //DXYN: Draws a sprite at coordinate (VX, VY) ...ETC.
	{
		unsigned short x = V[(currentOpcode & 0x0F00) >> 8];
		unsigned short y = V[(currentOpcode & 0x00F0) >> 4];
		unsigned short height = currentOpcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++) {

			pixel = memory[ir + yline];

			for (int xline = 0; xline < 8; xline++) {
				if ((pixel & (0x80 >> xline)) != 0) {
					if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
						V[0xF] = 1;
					}
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}
		drawFlag = true;
		pc += 2;
	}
		break;

	case 0xE000:
		switch (currentOpcode & 0x000F) {

		case 0x000E: //EX9E: Skips the next instruction if the key stored in VX is pressed
			if (key[V[(currentOpcode & 0x0F00) >> 8]] == 1) {
				pc += 2;
			}
			pc += 2;
			break;

		case 0x0001: //EXA1: Skips the next instruction if the key stored in VX is not pressed. 
			if (key[V[(currentOpcode & 0x0F00) >> 8]] != 1) {
				pc += 2;
			}
			pc += 2;
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", currentOpcode);
			break;

		}
		break;


	case 0xF000:
		switch (currentOpcode & 0x00FF) {

		case 0x0007: //FX07: 	Sets VX to the value of the delay timer.

			V[(currentOpcode & 0x0F00) >> 8] = delay_timer;
			pc += 2;
			break;

		case 0x000A: //FX0A: A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event);
			for (int i = 0; i < sizeof(key); i++) {
				if (key[i] == 1) {
					
					V[(currentOpcode & 0x0F00) >> 8] = i;
					pc += 2;
					break;
				}
			}
			break;

		case 0x0015: //FX15: Sets the delay timer to VX.
			delay_timer = V[(currentOpcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0018: //FX18: Sets the sound timer to VX.
			sound_timer = V[(currentOpcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x001E: //FX1E: Adds VX to I. VF is not affected
			if (ir + V[(currentOpcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
				V[0xF] = 1;
			else
				V[0xF] = 0;

			ir += V[(currentOpcode & 0x0F00) >> 8];
			pc += 2;
			break;

		case 0x0029: //FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
			
			switch (V[(currentOpcode & 0x0F00) >> 8]) {

			case 0x00:
				ir = 0x50 + 0;
				break;

			case 0x01:
				ir = 0x50 + 1;
				break;

			case 0x02:
				ir = 0x50 + 2;
				break;

			case 0x03:
				ir = 0x50 + 3;
				break;

			case 0x04:
				ir = 0x50 + 4;
				break;

			case 0x05:
				ir = 0x50 + 5;
				break;

			case 0x06:
				ir = 0x50 + 6;
				break;

			case 0x07:
				ir = 0x50 + 7;
				break;

			case 0x08:
				ir = 0x50 + 8;
				break;

			case 0x09:
				ir = 0x50 + 9;
				break;

			case 0x0A:
				ir = 0x50 + 10;
				break;

			case 0x0B:
				ir = 0x50 + 11;
				break;

			case 0x0C:
				ir = 0x50 + 12;
				break;

			case 0x0D:
				ir = 0x50 + 13;
				break;

			case 0x0E:
				ir = 0x50 + 14;
				break;

			case 0x0F:
				ir = 0x50 + 15;
				break;


			default:
				ir = 0x00;
				break;
			}

			pc += 2;
			break;

		case 0x0033: //FX33: Stores the binary-coded decimal representation of VX, with...ETC
			memory[ir] = V[(currentOpcode & 0x0F00) >> 8] / 100;
			memory[ir + 1] = (V[(currentOpcode & 0x0F00) >> 8] / 10) % 10;
			memory[ir + 2] = (V[(currentOpcode & 0x0F00) >> 8] % 100) % 10;
			pc += 2;
			break;

		case 0x0055: //FX55: Stores V0 to VX (including VX) in memory starting at address I. The...ETC

			for (int i = 0; i <= (currentOpcode & 0x0F00) >> 8; i++) {
				memory[ir + i] = V[i];
			}

			ir += ((currentOpcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;

		case 0x0065: //FX65:Fills V0 to VX (including VX) with values from memory starting...ETC
			for (int i = 0; i <= (currentOpcode & 0x0F00) >> 8; i++) {
				V[i] = memory[ir + i];
			}

			ir += ((currentOpcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", currentOpcode);
			break;

		}
		break;
	
	default:
		printf("Unknown opcode [0x0000]: 0x%X\n", currentOpcode);
		break;
	
	}
	
	// Execute Opcode
	
	// Update timers
	if (delay_timer > 0) {
		--delay_timer;
	}
	if (sound_timer > 0){
		if (sound_timer == 1) {
			printf("BEEP!\n");
		}
		--sound_timer;
	}
}