//
// Test file for the DCPU-16 compiler.
//

#include <screen.m>

void main()
{
	int val;
	val = a(5);
	scrn_setc('H',  0, 0);
	scrn_setc('e',  1, 0);
	scrn_setc('l',  2, 0);
	scrn_setc('l',  3, 0);
	scrn_setc('o',  4, 0);
	scrn_setc(' ',  5, 0);
	scrn_setc('W',  6, 0);
	scrn_setc('o',  7, 0);
	scrn_setc('r',  8, 0);
	scrn_setc('l',  9, 0);
	scrn_setc('d', 10, 0);
	if (val == 10)
		scrn_setc('!', 11, 0);
	else
		scrn_setc('#', 11, 0);
}

int a(int b)
{
	return 5 + b;
}
