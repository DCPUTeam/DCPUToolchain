/**

    File:       sdp.h

    Project:    DCPU-16 Tools
    Component:  Debugger

    Authors:    José Manuel Díez

    Description:    Socket debugging protocol header.

**/

#ifdef FEATURE_SDP

#ifndef __SDP_H
#define __SDP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <dcpu.h>

#define BUFSIZE 1024
#define PORT 22348

#define SDP_EMULATOR_INFORMATION    0x0
#define SDP_MACHINE_STATE       0x1
#define SDP_MACHINE_SET_STATE       0x2
#define SDP_CLIENT_DISCONNECT       0xFE

#define SDP_REQUEST_CONFIRMATION    0x20

#define SDP_MAX_PAYLOAD         1000
#define SDP_MAX_STRING          500

typedef struct
{
    uint8_t identifier;
    uint32_t length;
    unsigned char* payload;
} sdp_packet;

typedef struct
{
    uint16_t length;
    char* string;
} p_string;

void* ddbg_sdp_thread(vm_t* vm);

#endif

#endif
