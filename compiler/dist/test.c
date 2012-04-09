//
// Test file for the DCPU-16 compiler.
//

void subcall()
{
}

int set(char chr, int x, int y)
{
	int mem = 0x8000 + x + y * 32;
	*mem = chr + 0x7000;
	subcall();
	return chr;
}

void main()
{
	set('H', 0, 0);
	set('e', 1, 0);
	set('l', 2, 0);
	set('l', 3, 0);
	set('o', 4, 0);
	set(' ', 5, 0);
	set('0', 6, 0);
	set('x', 7, 0);
	set('1', 8, 0);
	set('0', 9, 0);
	set('c', 10, 0);

	set('T', 0, 1);
	set('e', 1, 1);
	set('s', 2, 1);
	set('t', 3, 1);
}