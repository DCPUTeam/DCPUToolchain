//
// DCPU-16 Libraries
//
// Screen utilities.
//

void scrn_setc(char chr, int x, int y)
{
	int mem = 0x8000 + x + y * 32;
	*mem = chr;
}

void scrn_sets(char* msg, int x, int y)
{
	int i = 0;
	for (i = 0; *(msg + i) != '\0'; i += 1)
		scrn_setc(*(msg + i) + 0xF000, x + i, y);
}