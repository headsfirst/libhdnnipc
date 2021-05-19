/* Libhdnnipc - Library for Neural Network Inter-Process Communication
 * Copyright 2018+ Janroel Koppen
 *
 */


#include "hdnnipc_Server.h"
#include "hdnnipc_Base.h"

#include <sys/types.h>
#include <sys/socket.h> // for: accept, struct sockaddr
#include <sys/un.h> // for: struct sockaddr_un

	
hdnnipc_Server* hdnnipc_Server_init(hdnnipc_Server* me, unsigned int ctype, const char* path) {
	if (me == NULL) {
		if ((me = (hdnnipc_Server*) calloc(1, sizeof(hdnnipc_Server))) == NULL) { goto _FAIL_; }
	}
	// init base
	if (hdnnipc_Base_init((hdnnipc_Base*)me, ctype, path, NULL) != (hdnnipc_Base*)me) { me = NULL; goto _FAIL_; } // NB!

	// accept?

	return me;
_FAIL_:
	return hdnnipc_Server_exit(me);
}

hdnnipc_Server* hdnnipc_Server_exit(hdnnipc_Server* me) {
	if (me != NULL) {
		// exit base
		if (hdnnipc_Base_exit((hdnnipc_Base*)me) != (hdnnipc_Base*)me) { me = NULL; } // NB!
	}
	
	return me;
}

hdnnipc_Server* hdnnipc_Server_copy(hdnnipc_Server* me, hdnnipc_Server* copy) {
	if (me == NULL) { goto _FAIL_; } // bail out
	if (me == copy) return copy; // bail out

	// (re-)init
	if ((copy = hdnnipc_Server_init(copy, me->base.ctype, me->base.path)) == NULL) { goto _FAIL_; }

	// copy base
	if (hdnnipc_Base_copy((hdnnipc_Base*)me, (hdnnipc_Base*)copy) != (hdnnipc_Base*)copy) { copy = NULL; goto _FAIL_; } // NB!

	return copy;
_FAIL_:
	return hdnnipc_Server_exit(copy);
}

int hdnnipc_Server_setCtrlBufferSize(hdnnipc_Server* me, size_t size) { 
	return hdnnipc_Base_setCtrlBufferSize((hdnnipc_Base*)me, size);
}

int hdnnipc_Server_setDataInBufferSize(hdnnipc_Server* me, size_t size) {
	return hdnnipc_Base_setDataInBufferSize((hdnnipc_Base*)me, size);
}

int hdnnipc_Server_setDataOutBufferSize(hdnnipc_Server* me, size_t size) {
	return hdnnipc_Base_setDataOutBufferSize((hdnnipc_Base*)me, size);
}

char* hdnnipc_Server_getCtrlBuffer(hdnnipc_Server* me) {
	return hdnnipc_Base_getCtrlBuffer((hdnnipc_Base*)me);
}

char* hdnnipc_Server_getDataInBuffer(hdnnipc_Server* me) {
	return hdnnipc_Base_getDataInBuffer((hdnnipc_Base*)me);
}

char* hdnnipc_Server_getDataOutBuffer(hdnnipc_Server* me) {
	return hdnnipc_Base_getDataOutBuffer((hdnnipc_Base*)me);
}

int hdnnipc_Server_accept(hdnnipc_Server* me, int fd, char** preamble_p) {
	int ret = 0;
	struct sockaddr addr;
	socklen_t addrlen;

	switch (me->base.ctype) {
		default:
		case 0x554e4958: // UNIX
		me->base.fd = accept(fd, &addr, &addrlen);
		break;
	}

	// preamble
	if (hdnnipc_Base_recvPreamble((hdnnipc_Base*)me) < 0) { ret = -3; goto _EXIT_; }
	// parse it?!?

_EXIT_:
	return ret;
}

int hdnnipc_Server_send(hdnnipc_Server* me) {
	return hdnnipc_Base_send((hdnnipc_Base*)me);
}

int hdnnipc_Server_recv(hdnnipc_Server* me) {
	return hdnnipc_Base_recv((hdnnipc_Base*)me);
}
