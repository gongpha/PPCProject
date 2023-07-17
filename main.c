#include "maindef.h"

// do not expect the oop-ish here

int main(int argc, char** argv)
{
	// pretend we were in 1996
	game__init();
	game__init_args(argc, argv);
	if (game__start()) return -1;
	game__loop();

	return 0;
}
