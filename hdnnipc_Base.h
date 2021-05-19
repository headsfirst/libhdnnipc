/* Libhdnnipc - Library for Neural Network Inter-Process Communication
 * Copyright 2018+ Janroel Koppen
 *
 */
#ifndef _HDNNIPC_BASE_H_
#define _HDNNIPC_BASE_H_
#ifdef __cplusplus
extern "C" {
#endif


#include <sys/types.h> // for size_t


typedef struct _hdnnipc_Base {
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
} hdnnipc_Base;

hdnnipc_Base* hdnnipc_Base_init(hdnnipc_Base* me, unsigned int ctype, const char* path, const char* preamble);
hdnnipc_Base* hdnnipc_Base_exit(hdnnipc_Base* me);
hdnnipc_Base* hdnnipc_Base_copy(hdnnipc_Base* me, hdnnipc_Base* copy);

int hdnnipc_Base_setCtrlBufferSize(hdnnipc_Base* me, size_t size);
int hdnnipc_Base_setDataInBufferSize(hdnnipc_Base* me, size_t size);
int hdnnipc_Base_setDataOutBufferSize(hdnnipc_Base* me, size_t size);

char* hdnnipc_Base_getCtrlBuffer(hdnnipc_Base* me);
char* hdnnipc_Base_getDataInBuffer(hdnnipc_Base* me);
char* hdnnipc_Base_getDataOutBuffer(hdnnipc_Base* me);

int hdnnipc_Base_sendPreamble(hdnnipc_Base* me);
int hdnnipc_Base_recvPreamble(hdnnipc_Base* me);

int hdnnipc_Base_send(hdnnipc_Base* me);
int hdnnipc_Base_recv(hdnnipc_Base* me);


#ifdef __cplusplus
}
#endif
#endif // _HDNNIPC_BASE_H_