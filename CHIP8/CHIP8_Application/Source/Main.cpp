#pragma region DEFINE/INCLUDE
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "CHIP8Emulator.h"  
#pragma endregion


int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return -1;
	}

	chipotto::Emulator emulator;

	if (emulator.IsValid())
	{
		emulator.LoadFromFile("C:\\Users\\Rene\\Desktop\\c8games\\TICTAC");
		while (true)
		{
			if (!emulator.Tick())
			{
				break;
			}
		}
	}

	SDL_Quit();
	return 0;
}