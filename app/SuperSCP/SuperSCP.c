#include "SuperSCP.h"

static void SuperSCP_send_buffer();
static void SuperSCPRecvInnerCallback();
static void SuperSCPRecvInnerCallback();
static Buffer send_buffer_super_scp;
static Buffer recv_buffer_super_scp;
static int SuperSCPEndAndStartChar = '#';
static int SuperSCPRevState=SUPERSCP_REV_OVER_AND_WAIT_START;

__attribute__((weak)) byte __send_data_super_scp[SUPERSCP_DEFAULT_BUFFER_SIZE];
__attribute__((weak)) byte __recv_data_super_scp[SUPERSCP_DEFAULT_BUFFER_SIZE];

// #define SuperSCP_ESCAPE_CHAR_LEN 2
// static char SuperSCP_control_byte_buf[SuperSCP_ESCAPE_CHAR_LEN] = {0};
void SuperSCP_init(int send_capacity,int recv_capacity)
{
    buffer_setmemory(&send_buffer_super_scp, __send_data_super_scp, send_capacity);
    buffer_setmemory(&recv_buffer_super_scp, __recv_data_super_scp, recv_capacity);
    SuperSCPRevState = SUPERSCP_REV_OVER_AND_WAIT_START;
}

void SuperSCP_default_init()
{
    buffer_setmemory(&send_buffer_super_scp, __send_data_super_scp, SUPERSCP_DEFAULT_BUFFER_SIZE);
    buffer_setmemory(&recv_buffer_super_scp, __recv_data_super_scp,SUPERSCP_DEFAULT_BUFFER_SIZE);
    SuperSCPRevState = SUPERSCP_REV_OVER_AND_WAIT_START;
}



static void SuperSCP_send_buffer()
{
    int i;
    for (i = 0; i < send_buffer_super_scp.size; i++)
    {
        SuperSCP_putchar(send_buffer_super_scp.data[i]);
    }
}

static void SuperSCPRecvInnerCallback()
{
    SuperSCPRecvCallback(recv_buffer_super_scp.data, recv_buffer_super_scp.size);
    buffer_clear(&recv_buffer_super_scp);
}

static void SuperSCPErrorInnerCallback(SuperSCPERROR error_code)
{
    SuperSCPErrorCallback(error_code);
    buffer_clear(&recv_buffer_super_scp);
}
/**
 * @brief Get the Super S C P End And Start Char object
 * 
 * @return int 
 */
int getSuperSCPEndAndStartChar(){
    return SuperSCPEndAndStartChar;
}
/**
 * @brief Set the Super S C P End And Start Char object.
 * 
 * @param end_and_start_char 
 * @return int current end and start char
 */
int setSuperSCPEndAndStartChar(int end_and_start_char){
    SuperSCPEndAndStartChar = end_and_start_char;
    return SuperSCPEndAndStartChar;
}


/*
Return:
    success: send data length
    fail: SuperSCP error code
*/
int SuperSCP_send(byte *data, int len)
{
    int i;
    //1. empty buffer
    buffer_clear(&send_buffer_super_scp);
    //2. push all data to buffer,check is buffer contain SuperSCPEndAndStartChar
    for (i = 0; i < len; i++)
    {
        if(data[i] == SuperSCPEndAndStartChar){
            return SUPERSCP_SEND_CONTAIN_CONTROL_BYTE_ERROR;
        }
        if (buffer_push(&send_buffer_super_scp, data[i]) == OVER_CAPACITY_ERROR)
        {
            return SUPERSCP_SEND_BUFFER_OVERFLOW_ERROR;
        }
    }
    //3. push the SuperSCPEndAndStartChar to buffer
    if(buffer_push(&send_buffer_super_scp, SuperSCPEndAndStartChar) == OVER_CAPACITY_ERROR){
        return SUPERSCP_SEND_BUFFER_OVERFLOW_ERROR;
    }
    //4. send message
    SuperSCP_send_buffer();
    return len;
}
/**
 * @brief When you just send one message,you can use this function force receiver to invoke SuperSCPRecvCallback.
 * 
 */
void SuperSCP_send_empty_msg(){
    SuperSCP_putchar(SuperSCPEndAndStartChar);
}
#include <stdio.h>
// SuperSCP receive state machine's states
void SuperSCP_parse(byte c)
{
    switch (SuperSCPRevState)
    {
    case SUPERSCP_REV_OVER_AND_WAIT_START:
        if (c != SuperSCPEndAndStartChar)
        {
            if (buffer_push(&recv_buffer_super_scp, c) == OVER_CAPACITY_ERROR)
            {
                SuperSCPRevState = SUPERSCP_REV_OVER_AND_WAIT_START;
                SuperSCPErrorInnerCallback(SUPERSCP_RECV_BUFFER_OVERFLOW_ERROR);
            }
            SuperSCPRevState = SUPERSCP_REVING;
        }
        break;
    case SUPERSCP_REVING:
        if (c == SuperSCPEndAndStartChar)
        {
            SuperSCPRevState = SUPERSCP_REV_OVER_AND_WAIT_START;
            SuperSCPRecvInnerCallback();
        }
        else
        {
            if (buffer_push(&recv_buffer_super_scp, c) == OVER_CAPACITY_ERROR)
            {
                SuperSCPRevState = SUPERSCP_REV_OVER_AND_WAIT_START;
                SuperSCPErrorInnerCallback(SUPERSCP_RECV_BUFFER_OVERFLOW_ERROR);
            }
        }
        break;
    default:
        break;
    }
}



__attribute__((weak)) void SuperSCPRecvCallback(byte *data, int len)
{
    __unimplemented
}
__attribute__((weak)) byte SuperSCP_putchar(byte c)
{
   __unimplemented
    return c;
}

__attribute__((weak)) void SuperSCPErrorCallback(SuperSCPERROR error_code){
    __unimplemented
}
