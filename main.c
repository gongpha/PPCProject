#include "maindef.h"

// do not expect the oop-ish here

int main(int argc, char** argv)
{
	// pretend we were in 1996
	Game_init();

	Game_init_args(argc, argv);
	if (Game_start()) return -1;

	world__load_bsp("abc");

	Game_loop();

	return 0;
}
