/* Libhdnnipc - Library for Neural Network Inter-Process Communication
 * Copyright 2018+  Heads First, Janroel Koppen
 *
 */
#ifndef _HDNNIPC_CLIENT_H_
#define _HDNNIPC_CLIENT_H_
#ifdef __cplusplus
extern "C" {
#endif


#include <sys/types.h> // for size_t


typedef struct _hdnnipc_Client {
	// properties
	unsigned int ctype; // connection type
	char* path; // socket path, '\0'-terminated
	char* preamble; // preamble, '\0'-terminated

	// state
	int fd; // file descriptor 
	char* cbuf; // ctrl buffer
	size_t cbuf_size; // ctrl buffer size
	char* dibuf; // data IN buffer
	size_t dibuf_size; // data IN buffer size
	char* dobuf; // data OUT buffer
	size_t dobuf_size; // data OUT buffer size
} hdnnipc_Client;

hdnnipc_Client* hdnnipc_Client_init(hdnnipc_Client* me, unsigned int ctype, const char* path, const char* preamble);
hdnnipc_Client* hdnnipc_Client_exit(hdnnipc_Client* me);
hdnnipc_Client* hdnnipc_Client_copy(hdnnipc_Client* me, hdnnipc_Client* copy);

int hdnnipc_Client_setCtrlBufferSize(hdnnipc_Client* me, size_t size);
int hdnnipc_Client_setDataInBufferSize(hdnnipc_Client* me, size_t size);
int hdnnipc_Client_setDataOutBufferSize(hdnnipc_Client* me, size_t size);

char* hdnnipc_Client_getCtrlBuffer(hdnnipc_Client* me);
char* hdnnipc_Client_getDataInBuffer(hdnnipc_Client* me);
char* hdnnipc_Client_getDataOutBuffer(hdnnipc_Client* me);

int hdnnipc_Client_send(hdnnipc_Client* me);
int hdnnipc_Client_recv(hdnnipc_Client* me);


#ifdef __cplusplus
}
#endif
#endif // _HDNNIPC_CLIENT_H_