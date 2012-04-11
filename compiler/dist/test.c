//
// Test file for the DCPU-16 compiler.
//

struct container_t
{
	char* string;
	int test;
};

void set(char chr, int x, int y)
{
	int mem = 0x8000 + x + y * 32;
	*mem = chr;
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
	struct container_t data;

	// Set string data.
	data.string = "This is my string!";
	data.test = 5;

	// Print
	print("Hello 0x10c!", 0, 0);
	print("How do you like", 0, 1);
	print("my C compiler? :D", 0, 2);
	print(data.string, data.test, 3);
}
