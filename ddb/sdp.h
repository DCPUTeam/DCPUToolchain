/**

	File:			sdp.h

	Project:		DCPU-16 Tools
	Component:		Debugger

	Authors:		José Manuel Díez

	Description:	Socket debugging protocol header.
	
**/

#include <stdio.h>
#include <stdlib.h>

#define SDP_EMULATOR_INFORMATION 	0x1
#define SDP_MACHINE_STATE 			0x0

#define SDP_MAGIC					0xC0DE

#define SDP_MAX_PAYLOAD				1000
#define SDP_MAX_STRING				500

typedef struct {
	uint16_t magic;
	uint8_t identifier;
	uint32_t length;
	unsigned char* payload;	
} sdp_packet;

typedef struct {
	uint8_t length;
	char* string;
} p_string;

void* ddbg_sdp_thread(void* vm);
