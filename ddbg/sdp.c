/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Debugger

	Authors:		José Manuel Díez

	Description:	Socket debugging protocol.

**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "sdp.h"

#define BUFSIZE 1024
#define PORT 56545

uint8_t protocol_version;
p_string* emulator_name;
p_string* emulator_version;

p_string* pstring_from_cstring(char* string) {
	p_string* res = malloc(sizeof(p_string));
	
	res->length = strlen(string) + 1;
	res->string = malloc(res->length);
	memcpy(res->string, string, res->length);
	return res;
}

char* cstring_from_pstring(p_string* string) {
	char* the_string = malloc(sizeof(char) * BUFSIZE);
	int length = string->length;
	
	memcpy(the_string, string->string, string->length);
	the_string[string->length+1] = '\0';
	
	return the_string;
}

unsigned char* serialize_sdp_packet(unsigned char* buffer, sdp_packet* out) {
	int orig_length = out->length;
	
	out->magic = htons(out->magic);
	memcpy(buffer, &out->magic, sizeof(out->magic));
	printf("%d\n", sizeof(out->magic));
	buffer += sizeof(out->magic);
	
	memcpy(buffer, &out->identifier, sizeof(out->identifier));
	buffer += sizeof(out->identifier);
	
	out->length = htons(out->length);
	memcpy(buffer, &out->length, sizeof(out->length));
	buffer += sizeof(out->length);
	
	memcpy(buffer, (void *) out->payload, orig_length);
	buffer += orig_length;
	
	return buffer;
}

void deserialize_sdp_packet(sdp_packet* in, char* buffer, int n) {
	int difference;

	memcpy(&in->magic, buffer, sizeof(in->magic));
	in->magic = ntohs(in->magic);
	buffer += sizeof(in->magic);
	
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
	
	printf("Wrote %d bytes\n", n);
}

unsigned char* serialize_pstring(p_string* string) {
	unsigned char* buffer = malloc(sizeof(unsigned char) * BUFSIZE);
	
	memcpy(buffer, &string->length, sizeof(string->length));
	memcpy(buffer + sizeof(string->length), string->string, string->length);
	
	return buffer;
}

void handle (int sock)
{
	int n;
	char buffer[BUFSIZE];
	unsigned char* payload_orig;
	
	p_string* test;
	sdp_packet* in = malloc(sizeof(sdp_packet));
	sdp_packet* out = malloc(sizeof(sdp_packet));
	
	
	for(;;) {
		bzero(buffer, BUFSIZE);
		
		n = read(sock, buffer, BUFSIZE);
		if(n > 0) {
			deserialize_sdp_packet(in, buffer, n);
			
	   		if(in->magic == SDP_MAGIC) {   			
	   			printf("Magic %p identifier %p length %p\n", in->magic, in->identifier, in->length);
	   			
	   			switch(in->identifier) {
	   				case SDP_EMULATOR_INFORMATION:		
	   					out->magic = SDP_MAGIC;
	   					out->identifier = SDP_EMULATOR_INFORMATION;
	   					out->length = sizeof(protocol_version) + emulator_name->length + emulator_version->length;
	   					out->payload = malloc(out->length);
						payload_orig = out->payload;
						
						memcpy(out->payload, &protocol_version, sizeof(protocol_version));
						out->payload += sizeof(protocol_version);
						
						memcpy(out->payload, serialize_pstring(emulator_name), emulator_name->length+1);
						out->payload += emulator_name->length+1;
						
						memcpy(out->payload, serialize_pstring(emulator_version), emulator_version->length+1);
						out->payload += emulator_version->length+1;
						
						out->payload = payload_orig;
						
						send_sdp_packet(sock, out);
	   			}
	   		} else {
	   			printf("Did not get SDP packet. Magic: %p \n", in->magic);
	   		}
		}
	}
}

void ddbg_handle_disconnection(int sgn) {
	int status;
	 
	signal(SIGCHLD, ddbg_handle_disconnection);
	wait(&status);
}


void tcp_server() {
	int parentfd,  childfd, clientlen, optval;
	struct sockaddr_in serveraddr, clientaddr;
	struct hostent *hostp;

	parentfd = socket(AF_INET, SOCK_STREAM, 0);
	if (parentfd < 0) 
		error("ERROR opening socket");

	optval = 1;
	setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short) PORT);

	bind(parentfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)); 
	listen(parentfd, 5);

	clientlen = sizeof(clientaddr);
	while (1) {
		childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
    
		if(fork() == 0) {
			close(parentfd);
			handle(childfd);
			exit(0);
		} else {
			close(childfd);
		}
    }
}

void* ddbg_sdp_thread(void* vm) {
	protocol_version = 1;
	emulator_name = pstring_from_cstring("DCPU Toolchain Emu");
	emulator_version = pstring_from_cstring("v13.37");
	
	signal(SIGCHLD, ddbg_handle_disconnection);
	tcp_server();
}
