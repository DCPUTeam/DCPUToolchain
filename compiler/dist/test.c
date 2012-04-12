//
// Test file for the DCPU-16 compiler.
//

int globalInt;
int anotherInt;
int thirdInt;

void (*func)(char* msg, int x, int y);

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
	for (i = 0; *(msg + i) != '\0'; i += 1)
		set(*(msg + i), x + i, y);
}

void print_in_red(char* msg, int x, int y)
{
	int i = 0;
	for (i = 0; *(msg + i) != '\0'; i += 1)
		set((*(msg + i) + 0x3000), x + i, y);
}

void main()
{
	struct container_t data;
	int five;

	// Set a global variable.
	globalInt = 2;
	anotherInt = 3;
	thirdInt = 0;

	// Set a local variable to the value of a global variable.
	five = globalInt + anotherInt;

	// Set string data.
	data.string = "This is my string!";
	data.test = 5;

	// Set function pointer.
	func = &print;
	
	// Print
	print("Hello 0x10c!", 0, 0);
	func("How do you like", 0, 1);
	func("my C compiler? :D", 0, 2);
	func(data.string, data.test, 3);
	func("Is it on line 6?", 0, five);

	// Change colour!
	func = &print_in_red;
	func("Now with extra red!", 0, 6);
}
