//
// DCPU-16 Libraries
//
// Screen utilities.
//

int* scrn_getmempos(int x, int y)
{
	return 0x8000 + x + y * 32;
}

void scrn_setc(char c, int x, int y)
{
	int* mem = scrn_getmempos(x, y);
	*mem = 0x7000 + c;
}