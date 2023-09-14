#include "maindef.h"

// do not expect the oop-ish here

int main(int argc, char** argv)
{
	// pretend we were in 1996
	Game_init();

	Game_init_args(argc, argv);
	if (Game_start()) return -1;

	World_load_bsp("it_building");

	Game_loop();

	return 0;
}
