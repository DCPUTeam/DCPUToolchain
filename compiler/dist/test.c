//
// Test file for the DCPU-16 compiler.
//

void set(char chr, int x, int y)
{
	int mem = 0x8000 + x + y * 32;
	*mem = chr + 0x7000;
}

void print(char* msg, int x, int y)
{
	int i = 0;
	//while (*(msg + i) != '\0')
	for (i = 0; *(msg + i) != '\0'; i += 1)
		set(*(msg + i), x + i, y);
}

void main()
{
	print("Hello 0x10c!", 0, 0);
	print("How do you like", 0, 1);
	print("my C compiler? :D", 0, 2);
}
