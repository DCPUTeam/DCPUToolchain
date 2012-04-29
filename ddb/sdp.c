/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Debugger

	Authors:		José Manuel Díez

	Description:	Socket debugging protocol.

**/

#ifdef FEATURE_SDP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h> 
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include <pthread.h>

#include <dcpu.h>

#include "sdp.h"
#include "packets.h"

uint8_t protocol_version;
p_string* emulator_name;
p_string* emulator_version;
int parentfd, childfd;
vm_t* vm;

p_string* pstring_from_cstring(char* string) {
	p_string* res = malloc(sizeof(p_string));
	
	res->length = strlen(string);
	printf("length: %d", res->length);
	res->string = malloc(res->length + 1);
	memcpy(res->string, string, res->length + 1);
	printf(", string: %s\n", res->string);
	return res;
}

char* cstring_from_pstring(p_string* string) {
	char* the_string = malloc(sizeof(char) * BUFSIZE);
	int length = string->length;
	
	memcpy(the_string, string->string, string->length);
	the_string[string->length+1] = '\0';
	
	return the_string;
}


int pstr_length(p_string* string) {
	return string->length + 2;
}

void handle (int sock)
{
	
	int n;
	char buffer[BUFSIZE];
	unsigned char* payload_orig;
	uint16_t tmp;
	uint32_t tmp32;
	uint8_t tmp8;
	
	sdp_packet* in = malloc(sizeof(sdp_packet));
	sdp_packet* out = malloc(sizeof(sdp_packet));
	
	protocol_version = 1;
	
	for(;;) {
		bzero(buffer, BUFSIZE);
		
		n = read(sock, buffer, BUFSIZE);
		if(n > 0) {
			deserialize_sdp_packet(in, buffer, n);
			printf("requested mode: %d\n", in->identifier);
			
   			switch(in->identifier) {
   				case SDP_EMULATOR_INFORMATION:		
   					out->identifier = SDP_EMULATOR_INFORMATION;
   					out->length = sizeof(protocol_version) + pstr_length(emulator_name) + pstr_length(emulator_version);
   					out->payload = malloc(out->length);
					payload_orig = out->payload;
				
					memcpy(out->payload, &protocol_version, sizeof(protocol_version));
					out->payload += sizeof(protocol_version);
				
					memcpy(out->payload, serialize_pstring(emulator_name), pstr_length(emulator_name));
					out->payload += pstr_length(emulator_name);
				
					memcpy(out->payload, serialize_pstring(emulator_version), pstr_length(emulator_version));
					out->payload += pstr_length(emulator_version);
				
					out->payload = payload_orig;	
					send_sdp_packet(sock, out);
					
					break;
				case SDP_MACHINE_STATE:
					out->identifier = SDP_MACHINE_STATE;
					out->length = sizeof(uint8_t) + sizeof(uint16_t) * 13 + sizeof(uint32_t) + sizeof(uint8_t);
					out->payload = malloc(out->length);
					payload_orig = out->payload;
					
					tmp8 = vm->halted;
					memcpy(out->payload, &tmp8, sizeof(uint8_t));
					out->payload += sizeof(uint8_t);
					
					tmp = htons(vm->registers[REG_A]);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->registers[REG_B]);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->registers[REG_C]);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->registers[REG_X]);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->registers[REG_Y]);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->registers[REG_Z]);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->registers[REG_I]);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->registers[REG_J]);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->pc);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->sp);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->ex);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(vm->ia);
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp = htons(100); // kHz
					memcpy(out->payload, &tmp, sizeof(uint16_t));
					out->payload += sizeof(uint16_t);
					
					tmp32 = htonl(1337);
					memcpy(out->payload, &tmp32, sizeof(uint32_t));
					out->payload += sizeof(uint32_t);
					
					tmp8 = 0xFF;
					memcpy(out->payload, &tmp8, sizeof(uint8_t));
					
					out->payload = payload_orig;
					send_sdp_packet(sock, out);
					
					break;
					
				case SDP_MACHINE_SET_STATE:
					memcpy(&tmp8, in->payload, sizeof(tmp8));
					in->payload += sizeof(tmp8);
					vm->halted = tmp8;
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->registers[REG_A] = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->registers[REG_B] = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->registers[REG_C] = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->registers[REG_X] = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->registers[REG_Y] = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->registers[REG_Z] = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->registers[REG_I] = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->registers[REG_J] = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->pc = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->sp = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->ex = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					vm->ia = ntohs(tmp);
					
					memcpy(&tmp, in->payload, sizeof(tmp));
					in->payload += sizeof(tmp);
					
					out->identifier = SDP_REQUEST_CONFIRMATION;
					out->length = 0;
					send_sdp_packet(sock, out);
					break;
					
   			}
		} else {
			close(sock);
			exit(0);
		}
	}
}

void ddbg_handle_disconnection(int sgn) {
	int status;
	 
	signal(SIGCHLD, ddbg_handle_disconnection);
	close(childfd);
	wait(&status);
}

void ddbg_handle_sigterm(int sgn) {
	int status;
	signal(SIGCHLD, NULL); // ignore sigchld

	close(childfd);
	close(parentfd);
	wait(&status);
	exit(0);
}


void tcp_server() {
	int clientlen, optval;
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
		pthread_testcancel();
		childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
    
		if(fork() == 0) {
			handle(childfd);
			close(childfd);
			exit(0);
		} else {
			close(childfd);
		}
    }
}

void* ddbg_sdp_thread(vm_t* vm) {
	emulator_name = pstring_from_cstring("DCPU Toolchain Emu");
	emulator_version = pstring_from_cstring("v13.37");
	
	signal(SIGCHLD, ddbg_handle_disconnection);
	signal(SIGTERM, ddbg_handle_sigterm);
	tcp_server();
}

#endif