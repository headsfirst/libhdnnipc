/* Libhdnnipc - Library for Neural Network Inter-Process Communication
 * Copyright 2018+ Janroel Koppen
 *
 */
#ifndef _HDNNIPC_SERVER_H_
#define _HDNNIPC_SERVER_H_
#ifdef __cplusplus
extern "C" {
#endif


#include "hdnnipc_Base.h"

#include <stdlib.h> // for size_t

typedef struct _hdnnipc_Server {
	hdnnipc_Base base; // NB keep this the first member!
} hdnnipc_Server;

hdnnipc_Server* hdnnipc_Server_init(hdnnipc_Server* me, unsigned int ctype, const char* path);
hdnnipc_Server* hdnnipc_Server_exit(hdnnipc_Server* me);
hdnnipc_Server* hdnnipc_Server_copy(hdnnipc_Server* me, hdnnipc_Server* copy);

int hdnnipc_Server_setCtrlBufferSize(hdnnipc_Server* me, size_t size);
int hdnnipc_Server_setDataInBufferSize(hdnnipc_Server* me, size_t size);
int hdnnipc_Server_setDataOutBufferSize(hdnnipc_Server* me, size_t size);

char* hdnnipc_Server_getCtrlBuffer(hdnnipc_Server* me);
char* hdnnipc_Server_getDataInBuffer(hdnnipc_Server* me);
char* hdnnipc_Server_getDataOutBuffer(hdnnipc_Server* me);

int hdnnipc_Server_accept(hdnnipc_Server* me, int fd, char** preamble);
int hdnnipc_Server_send(hdnnipc_Server* me);
int hdnnipc_Server_recv(hdnnipc_Server* me);


#ifdef __cplusplus
}
#endif
#endif // _HDNNIPC_SERVER_H_