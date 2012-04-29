/**

	File:			packets.c

	Project:		DCPU-16 Tools
	Component:		Debugger

	Authors:		José Manuel Díez

	Description:	Miscellaneous packet functions.
	
**/

#ifdef FEATURE_SDP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "sdp.h"
#include "packets.h"

unsigned char* serialize_sdp_packet(unsigned char* buffer, sdp_packet* out) {
	int orig_length = out->length;
	
	memcpy(buffer, &out->identifier, sizeof(out->identifier));
	buffer += sizeof(out->identifier);
	
	out->length = htonl(out->length);
	memcpy(buffer, &out->length, sizeof(out->length));
	buffer += sizeof(out->length);
	
	memcpy(buffer, (void *) out->payload, orig_length);
	buffer += orig_length;
	
	return buffer;
}

void deserialize_sdp_packet(sdp_packet* in, char* buffer, int n) {
	int difference;

	printf("read %d bytes\n", n);

	memcpy(&in->identifier, buffer, sizeof(in->identifier));
	buffer += sizeof(in->identifier);
	
	memcpy(&in->length, buffer, sizeof(in->length));
	buffer += sizeof(in->length);
	in->length = ntohs(in->length);
	
	difference = (n - in->length) + 2;
	in->payload = malloc(difference);
	memcpy(in->payload, buffer, difference);
}

void send_sdp_packet(int sock, sdp_packet* out)
{

	unsigned char serialization_buffer[BUFSIZE];
 	unsigned char* ptr;
	int n;

	ptr = serialize_sdp_packet(serialization_buffer, out);
	n = write(sock, serialization_buffer, ptr - serialization_buffer);
	
	if(n == -1) {
		printf("Closing sock.\n");
		close(sock);
		exit(0);
	}
	
	printf("Wrote %d bytes\n", n);
}

unsigned char* serialize_pstring(p_string* string) {
	unsigned char* buffer = malloc(sizeof(unsigned char) * BUFSIZE);
	uint16_t serialized_length = htons(string->length);
	
	
	memcpy(buffer, &serialized_length, sizeof(serialized_length));
	memcpy(buffer + sizeof(serialized_length), string->string, string->length);
	
	return buffer;
}

#endif