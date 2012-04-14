/**

	File:			hwioascii.c

	Project:		DCPU-16 Tools
	Component:		Emulator

	Authors:		James Rhodes

	Description:	Defines the vm_hw_io_ascii_get_map function which
					maps libTCOD values to the ASCII equivalents.

**/

#include "hwioascii.h"

uint16_t vm_hw_io_ascii_get_map(TCOD_key_t key)
{
	uint16_t ascii = 0;
	switch (key.vk)
	{
	case TCODK_ESCAPE:		ascii = 0x1B;	break;
	case TCODK_BACKSPACE:	ascii = 0x08;	break;
	case TCODK_TAB:			ascii = 0x09;	break;
	case TCODK_ENTER:		ascii = 0x0A;	break;
	case TCODK_SHIFT:						break;
	case TCODK_CONTROL:						break;
	case TCODK_ALT:							break;
	case TCODK_PAUSE:						break;
	case TCODK_CAPSLOCK:					break;
	case TCODK_PAGEUP:						break;
	case TCODK_PAGEDOWN:					break;
	case TCODK_END:							break;
	case TCODK_HOME:						break;
	case TCODK_UP:			ascii = 0x26;	break;
	case TCODK_LEFT:		ascii = 0x25;	break;
	case TCODK_RIGHT:		ascii = 0x27;	break;
	case TCODK_DOWN:		ascii = 0x28;	break;
	case TCODK_PRINTSCREEN:					break;
	case TCODK_INSERT:						break;
	case TCODK_DELETE:						break;
	case TCODK_LWIN:						break;
	case TCODK_RWIN:						break;
	case TCODK_APPS:						break;
	case TCODK_KPADD:		ascii = '+';	break;
	case TCODK_KPSUB:		ascii = '-';	break;
	case TCODK_KPDIV:		ascii = '/';	break;
	case TCODK_KPMUL:		ascii = '*';	break;
	case TCODK_KPDEC:		ascii = '.';	break;
	case TCODK_KPENTER:		ascii = 0x0A;	break;
	case TCODK_F1:							break;
	case TCODK_F2:							break;
	case TCODK_F3:							break;
	case TCODK_F4:							break;
	case TCODK_F5:							break;
	case TCODK_F6:							break;
	case TCODK_F7:							break;
	case TCODK_F8:							break;
	case TCODK_F9:							break;
	case TCODK_F10:							break;
	case TCODK_F11:							break;
	case TCODK_F12:							break;
	case TCODK_NUMLOCK:						break;
	case TCODK_SCROLLLOCK:					break;
	case TCODK_SPACE:						break;
	case TCODK_0:			ascii = '0';	break;
	case TCODK_1:			ascii = '1';	break;
	case TCODK_2:			ascii = '2';	break;
	case TCODK_3:			ascii = '3';	break;
	case TCODK_4:			ascii = '4';	break;
	case TCODK_5:			ascii = '5';	break;
	case TCODK_6:			ascii = '6';	break;
	case TCODK_7:			ascii = '7';	break;
	case TCODK_8:			ascii = '8';	break;
	case TCODK_9:			ascii = '9';	break;
	case TCODK_KP0:			ascii = '0';	break;
	case TCODK_KP1:			ascii = '1';	break;
	case TCODK_KP2:			ascii = '2';	break;
	case TCODK_KP3:			ascii = '3';	break;
	case TCODK_KP4:			ascii = '4';	break;
	case TCODK_KP5:			ascii = '5';	break;
	case TCODK_KP6:			ascii = '6';	break;
	case TCODK_KP7:			ascii = '7';	break;
	case TCODK_KP8:			ascii = '8';	break;
	case TCODK_KP9:			ascii = '9';	break;
	case TCODK_CHAR:		ascii = key.c;	break;
	}
	return ascii;
}