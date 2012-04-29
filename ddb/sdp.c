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
#include <dcpuhook.h>

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

void sdp_on_cycle(vm_t* vm, uint16_t pos)
{
	printf("hook.on.cycle\n");
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
	
	protocol_version = 2;
	
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
					out->length = sizeof(uint8_t) + sizeof(uint16_t) * 12 + sizeof(uint32_t) * 2 + sizeof(uint8_t);
					out->payload = malloc(out->length);
					payload_orig = out->payload;
					
					if(vm->halted == true) write_uint8(out, 1); else write_uint8(out, 0);
					write_uint16(out, vm->registers[REG_A]);
					write_uint16(out, vm->registers[REG_B]);
					write_uint16(out, vm->registers[REG_C]);
					write_uint16(out, vm->registers[REG_X]);
					write_uint16(out, vm->registers[REG_Y]);
					write_uint16(out, vm->registers[REG_Z]);
					write_uint16(out, vm->registers[REG_I]);
					write_uint16(out, vm->registers[REG_J]);					
					write_uint16(out, vm->pc);
					write_uint16(out, vm->sp);
					write_uint16(out, vm->ex);
					write_uint16(out, vm->ia);
					write_uint32(out, 100 * 1000);
					write_uint32(out, 1337);
					write_uint8(out, 0xFF);
					
					out->payload = payload_orig;
					send_sdp_packet(sock, out);
					
					break;
					
				case SDP_MACHINE_SET_STATE:
					printf("machine_State\n");
					if(read_uint8(in) == 1) {
						vm->halted = false;
						//vm_execute(vm);
					}
					printf("machine_state\n");
					
					vm->registers[REG_A] = read_uint16(in);
					vm->registers[REG_B] = read_uint16(in);
					vm->registers[REG_C] = read_uint16(in);
					vm->registers[REG_X] = read_uint16(in);
					vm->registers[REG_Y] = read_uint16(in);
					vm->registers[REG_Z] = read_uint16(in);
					vm->registers[REG_I] = read_uint16(in);
					vm->registers[REG_J] = read_uint16(in);
					vm->pc = read_uint16(in);
					vm->sp = read_uint16(in);
					vm->ex = read_uint16(in);
					vm->ia = read_uint16(in);
					read_uint8(in);
					
					out->identifier = SDP_REQUEST_CONFIRMATION;
					out->length = 0;
					send_sdp_packet(sock, out);
					break;
				
				case SDP_CLIENT_DISCONNECT:
					close(sock);
					exit(0);
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
		printf("ohai\n");
		childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);
		printf("got child\n");
		if(fork() == 0) {
			handle(childfd);
			close(childfd);
			exit(0);
		} else {
			close(childfd);
		}
		pthread_testcancel();
    }
}

void* ddbg_sdp_thread(vm_t* vm_import) {
	vm = vm_import;
	emulator_name = pstring_from_cstring("DCPU Toolchain Emu");
	emulator_version = pstring_from_cstring("v13.37");
	
	//vm_hook_register(vm, &sdp_on_cycle, HOOK_ON_CYCLE);
	
	signal(SIGCHLD, ddbg_handle_disconnection);
	signal(SIGTERM, ddbg_handle_sigterm);
	tcp_server();
}

#endif