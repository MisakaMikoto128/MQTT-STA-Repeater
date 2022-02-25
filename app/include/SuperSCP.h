#ifndef _SUPER_SCP_H_
#define _SUPER_SCP_H_
#include "Buffer.h"

//cast char * to byte *
#define byte unsigned char
#define CAST_CHAR_PTR_TO_BYTE_PTR(ptr) (byte *)(ptr)

// SuperSCP receive state machine's states
#define SUPERSCP_REV_OVER_AND_WAIT_START 0
#define SUPERSCP_REVING 1
// SuperSCP frame control byte (The frame delimiter)
static int SuperSCPEndAndStartChar;
#define containSuperSCPCtrolByte(c) ((c) == SuperSCPEndAndStartChar)

// A tag with an implementation function or variable
#define __unimplemented 
#define __implemented

//default buffer size
#define SUPERSCP_DEFAULT_BUFFER_SIZE 512

typedef int SuperSCPERROR;
// define SuperSCP's error code
#define SUPERSCP_SEND_CONTAIN_CONTROL_BYTE_ERROR -1
#define SUPERSCP_SEND_BUFFER_OVERFLOW_ERROR -2
#define SUPERSCP_RECV_BUFFER_OVERFLOW_ERROR -3

#define SUPERSCP_SEND_SUCCESS 0
// Externally provided methods
void SuperSCP_init(int send_capacity, int recv_capacity);
void SuperSCP_default_init();
int SuperSCP_send(byte *data, int len);
void SuperSCP_send_empty_msg();
void SuperSCP_parse(byte c);
int getSuperSCPEndAndStartChar();
int setSuperSCPEndAndStartChar(int end_and_start_char);
__unimplemented void SuperSCPRecvCallback(byte *data, int len);
__unimplemented void SuperSCPErrorCallback(SuperSCPERROR error_code);
__unimplemented byte SuperSCP_putchar(byte c);

#endif // _SUPER_SCP_H_