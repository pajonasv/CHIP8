#include "SDL.h"
#undef main
#include <iostream>
#include <time.h>

#include <chrono>
#include "chip8.h"

#ifdef _WIN32 || _WIN64
#include <Windows.h>
#else
#include <unistd.h>
#endif




void logSDLError();
bool setupSDL();


SDL_Window* win;
SDL_Renderer* ren;



int main(int argc, char* argv[]) {


	for (int i = 0; i < argc; i++) {
		std::cout << i << " " << argv[i] << std::endl;
	
	}
	if (argc < 2) {
		std::cout << "Error 0: open with file." << std::endl;
		return 1;

	}


	

	
	if (!setupSDL()) {
		return 1;
	}



	


	SDL_Event e;
	bool running = true;

	chip8 chip;

	chip.initialize();

	if (!chip.loadProgram(argv[1])) {
		return 1;
	}

	

	SDL_Color color{ 255,255,255,255 };
	SDL_RenderClear(ren);

	std::chrono::system_clock::time_point t;
	std::chrono::duration<double, std::milli> dur;
	int screenW = sizeof(chip.gfx) * 8;
	int pixelX;
	int pixelY;
	double cycleSpeed = CLOCKS_PER_SEC / 360;
	bool uncapFPS = false;

    //While application is running
    while (running){
		t = std::chrono::system_clock::now();
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0){
            //User requests quit
			switch (e.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_1:
					chip.key[0x01] = 1;
					
					break;
				case SDLK_2:

					chip.key[0x02] = 1;
					break;
				case SDLK_3:

					chip.key[0x03] = 1;
					break;
				case SDLK_4:

					chip.key[0x0C] = 1;
					break;
				case SDLK_q:

					chip.key[0x04] = 1;
					break;
				case SDLK_w:

					chip.key[0x05] = 1;
					break;
				case SDLK_e:

					chip.key[0x06] = 1;
					break;
				case SDLK_r:

					chip.key[0x0D] = 1;
					break;
				case SDLK_a:

					chip.key[0x07] = 1;
					break;
				case SDLK_s:

					chip.key[0x08] = 1;
					break;
				case SDLK_d:

					chip.key[0x09] = 1;
					break;
				case SDLK_f:

					chip.key[0x0E] = 1;
					break;
				case SDLK_z:

					chip.key[0x0A] = 1;
					break;
				case SDLK_x:

					chip.key[0x00] = 1;
					break;
				case SDLK_c:

					chip.key[0x0B] = 1;
					break;
				case SDLK_v:

					chip.key[0x0F] = 1;
					break;

				//colors
				case SDLK_KP_0:
					color = SDL_Color{255,255,255,255};
					break;

				case SDLK_KP_1:
					color = SDL_Color{255,0,0,255};
					break;

				case SDLK_KP_2:
					color = SDL_Color{0,255,0,255};
					break;

				case SDLK_KP_3:
					color = SDL_Color{0,0,255,255};
					break;

				case SDLK_KP_4:
					color = SDL_Color{255,0,255,255};
					break;

				case SDLK_KP_5:
					color = SDL_Color{255,255,0,255};
					break;

				case SDLK_BACKSPACE:
					chip.initialize();
					break;
				case SDLK_SPACE:
					uncapFPS = true;
					break;
				case SDLK_RSHIFT:
					if (chip.debugMode == false) {
						chip.debugMode = true;
					}
					else {
						chip.debugMode = false;
					}
					break;
				}
				//reset
				
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.sym) {
				case SDLK_1:
					chip.key[0x01] = 0;

					break;
				case SDLK_2:

					chip.key[0x02] = 0;
					break;
				case SDLK_3:

					chip.key[0x03] = 0;
					break;
				case SDLK_4:

					chip.key[0x0C] = 0;
					break;
				case SDLK_q:

					chip.key[0x04] = 0;
					break;
				case SDLK_w:

					chip.key[0x05] = 0;
					break;
				case SDLK_e:

					chip.key[0x06] = 0;
					break;
				case SDLK_r:

					chip.key[0x0D] = 0;
					break;
				case SDLK_a:

					chip.key[0x07] = 0;
					break;
				case SDLK_s:

					chip.key[0x08] = 0;
					break;
				case SDLK_d:

					chip.key[0x09] = 0;
					break;
				case SDLK_f:

					chip.key[0x0E] = 0;
					break;
				case SDLK_z:

					chip.key[0x0A] = 0;
					break;
				case SDLK_x:

					chip.key[0x00] = 0;
					break;
				case SDLK_c:

					chip.key[0x0B] = 0;
					break;
				case SDLK_v:

					chip.key[0x0F] = 0;
					break;
				case SDLK_SPACE:
					uncapFPS = false;
					break;
				}
				break;

			}
            

			
        }

		chip.emulateCycle();

		
		
		//render here
		if (chip.drawFlag) {


				SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
				SDL_RenderClear(ren);
				chip.clearScreenFlag = false;
				
				SDL_SetRenderDrawColor(ren,color.r,color.g,color.b,color.a);

				for (int i = 0; i < sizeof(chip.gfx); i++) {

					pixelY = (int)floor(i / 64);
					pixelX = (i)-(pixelY * 64);
					if (chip.gfx[i] == 1) {



						SDL_RenderFillRect(
							ren,
							new SDL_Rect{ pixelX * 10,pixelY * 10,10,10 }
						);

					}



				}
			SDL_RenderPresent(ren);
			chip.drawFlag = false;
		}

		dur = std::chrono::system_clock::now() - t;

		if (dur.count() < cycleSpeed && !uncapFPS) {
			Sleep((int)(cycleSpeed - dur.count()));
		}
	
    }

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);

    //Quit SDL subsystems
    SDL_Quit();

	return 0;
}


void logSDLError() {
	std::cout << " error: " << SDL_GetError() << std::endl;
}

bool setupSDL() {

	//initializing SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError();
		return false;
	}
	//window										   xpos, ypos, sizes,     flags 
	win = SDL_CreateWindow("Chip8", 100, 100, 640, 320, SDL_WINDOW_SHOWN );
	if (win == nullptr) {
		logSDLError();
		SDL_Quit();
		return false;
	}

	//renderer								window, index of rendering driver, flags 
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) {
		SDL_DestroyWindow(win);
		logSDLError();
		SDL_Quit();
		return false;
	}
	return true;
}