/**

	File:		packets.h

	Project:	DCPU-16 Tools
	Component:	Debugger

	Authors:	José Manuel Díez

	Description:	Header for packets.c

**/

#ifdef FEATURE_SDP

#ifndef __PACKETS_H
#define __PACKETS_H

#include <stdio.h>
#include <stdlib.h>

unsigned char* serialize_sdp_packet(unsigned char* buffer, sdp_packet* out);
void deserialize_sdp_packet(sdp_packet* in, char* buffer, int n);
void send_sdp_packet(int sock, sdp_packet* out);
unsigned char* serialize_pstring(p_string* string);

#endif

#endif
