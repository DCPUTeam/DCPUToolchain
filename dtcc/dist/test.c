//
// Test file for the DCPU-16 compiler.
//

#include "screen.m"

int globalInt;
int anotherInt;
int thirdInt;

void (*func)(char* msg, int x, int y) = &scrn_sets;

struct container_t
{
	char* string;
	int test;
};

void print_in_red(char* msg, int x, int y)
{
	int i = 0;
	for (i = 0; * (msg + i) != '\0'; i += 1)
		scrn_setc((*(msg + i) + 0xC000), x + i, y);
}

void print_in_yellow(char* msg, int x, int y)
{
	int i = 0;
	for (i = 0; * (msg + i) != '\0'; i += 1)
		scrn_setc((*(msg + i) + 0xE000), x + i, y);
}

int get_input()
{
	// Assumes legacy mode enabled in the emulator.
	return *0x9000;
}

void set_input(int val)
{
	// Assumes legacy mode enabled in the emulator.
	* 0x9000 = val;
}

void main()
{
	struct container_t data;
	int five;
	void (*local)(char * msg, int x, int y) = &scrn_sets;
	int test;

	// Set a global variable.
	globalInt = 2;
	anotherInt = 3;
	thirdInt = 0;

	// Set a local variable to the value of a global variable.
	five = globalInt + anotherInt;

	// Set string data.
	data.string = "This is my string!";
	data.test = 5;

	// Print
	scrn_sets("Hello 0x10c!", 0, 0);
	func("How do you like", 0, 1);
	func("my C compiler? :D", 0, 2);
	func(data.string, data.test, 3);
	func("Is it on line 6?", 0, five);

	// Change colour by changing the function pointer!
	func = &print_in_red;
	local = &print_in_yellow;
	func("Now with extra red!", 0, 6);
	local("Local function pointers work to!", 0, 7);

	while (true)
	{
		// Continually get input and set 0, 0 to it.
		test = get_input();
		set_input(0);
		if (test != 0)
			scrn_setc(test + 0xF000, 0, 0);
	}
}
