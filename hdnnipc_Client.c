/* Libhdnnipc - Library for Neural Network Inter-Process Communication
 * Copyright 2018+ Janroel Koppen
 *
 */


#include "hdnnipc_Client.h"
#include "hdnnipc_Base.h"

#include <string.h> // for: calloc, strncpy
#include <stdlib.h> // for: realloc, free
#include <unistd.h> // for: close
#include <sys/socket.h> // for: socket, connect, struct sockaddr
#include <sys/un.h> // for: struct sockaddr_un
#include <stdio.h> // for: snprintf
	
static int _hdnnipc_Client_resize(hdnnipc_Client* me, size_t cbuf_size, size_t dibuf_size, size_t dobuf_size);
static int _writeChunk(int fd, const char* buf, size_t buf_size);
static int _readChunk(int fd, char* buf, size_t buf_size);


hdnnipc_Client* hdnnipc_Client_init(hdnnipc_Client* me, unsigned int ctype, const char* path, const char* preamble) {
	if (me == NULL) {
		if ((me = (hdnnipc_Client*) calloc(1, sizeof(hdnnipc_Client))) == NULL) { goto _FAIL_; }
	}
	// init base
	if (hdnnipc_Base_init((hdnnipc_Base*)me, ctype, path, preamble) != (hdnnipc_Base*)me) { me = NULL; goto _FAIL_; } // NB!

	return me;
_FAIL_:
	return hdnnipc_Client_exit(me);
}

hdnnipc_Client* hdnnipc_Client_exit(hdnnipc_Client* me) {
	if (me != NULL) {
		// exit base
		if (hdnnipc_Base_exit((hdnnipc_Base*)me) != (hdnnipc_Base*)me) { me = NULL; } // NB!
	}
	
	return me;
}

hdnnipc_Client* hdnnipc_Client_copy(hdnnipc_Client* me, hdnnipc_Client* copy) {
	if (me == NULL) { goto _FAIL_; } // bail out
	if (me == copy) return copy; // bail out

	// (re-)init
	if ((copy = hdnnipc_Client_init(copy, me->base.ctype, me->base.path, me->base.preamble)) == NULL) { goto _FAIL_; }

	// copy base
	if (hdnnipc_Base_copy((hdnnipc_Base*)me, (hdnnipc_Base*)copy) != (hdnnipc_Base*)copy) { copy = NULL; goto _FAIL_; } // NB!

	return copy;
_FAIL_:
	return hdnnipc_Client_exit(copy);
}

int hdnnipc_Client_setCtrlBufferSize(hdnnipc_Client* me, size_t size) {
	return (me != NULL) ? _hdnnipc_Client_resize(me, size, me->base.dibuf_size, me->base.dobuf_size) : -1;
}

int hdnnipc_Client_setDataInBufferSize(hdnnipc_Client* me, size_t size) {
	if (me == NULL) return -1; // bail out

	return _hdnnipc_Client_resize(me, me->base.cbuf_size, size, me->base.dobuf_size);
}

int hdnnipc_Client_setDataOutBufferSize(hdnnipc_Client* me, size_t size) {
	if (me == NULL) return -1; // bail out

	return _hdnnipc_Client_resize(me, me->base.cbuf_size, me->base.dibuf_size, size);
}

char* hdnnipc_Client_getCtrlBuffer(hdnnipc_Client* me) {
	return (me != NULL) ? me->base.cbuf : NULL;
}

char* hdnnipc_Client_getDataInBuffer(hdnnipc_Client* me) {
	return (me != NULL) ? me->base.dibuf : NULL;
}

char* hdnnipc_Client_getDataOutBuffer(hdnnipc_Client* me) {
	return (me != NULL) ? me->base.dobuf : NULL;
}

int hdnnipc_Client_connect(hdnnipc_Client* me) {
	int ret = 0;
	struct sockaddr_un addr_unix;

	// connect
	if (me->base.fd >= 0) { close(me->base.fd); me->base.fd = -1; }
	switch (me->base.ctype) {
		default:
		case 0x554e4958: // UNIX
		if (me->base.path != NULL) {
			if ((me->base.fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) { ret = -1; goto _EXIT_; }
			memset(&addr_unix, 0, sizeof(addr_unix));
			addr_unix.sun_family = AF_UNIX;
			strncpy(addr_unix.sun_path, me->base.path, sizeof(addr_unix.sun_path)-1);
			if (connect(me->base.fd, (struct sockaddr*)&addr_unix, sizeof(addr_unix)) < 0) { ret = -2; goto _EXIT_; }
		}
		break;
	}

	// preamble
	if (hdnnipc_Base_sendPreamble((hdnnipc_Base*)me) < 0) { ret = -3; goto _EXIT_; }


_EXIT_:
	return ret;
}

int hdnnipc_Client_send(hdnnipc_Client* me) {
	return hdnnipc_Base_send((hdnnipc_Base*)me);
}

int hdnnipc_Client_recv(hdnnipc_Client* me) {
	return hdnnipc_Base_recv((hdnnipc_Base*)me);
}

static int _hdnnipc_Client_resize(hdnnipc_Client* me, size_t cbuf_size, size_t dibuf_size, size_t dobuf_size) {
	int ret = 0;
	void* tmp = NULL;

	if (me == NULL) return -1; // bail out

	// ctrl buffer
	if ((tmp = realloc(me->base.cbuf, cbuf_size * sizeof(char))) == NULL && cbuf_size > 0) { ret = -2; goto _EXIT_; }
	me->base.cbuf = (char*)tmp;
	me->base.cbuf_size = cbuf_size;
	
	// data buffers
	if ((tmp = realloc(me->base.dibuf, dibuf_size * sizeof(char))) == NULL && dibuf_size > 0) { ret = -2; goto _EXIT_; }
	me->base.dibuf = (char*)tmp;
	me->base.dibuf_size = dibuf_size;
	if ((tmp = realloc(me->base.dobuf, dobuf_size * sizeof(char))) == NULL && dobuf_size > 0) { ret = -2; goto _EXIT_; }
	me->base.dobuf = (char*)tmp;
	me->base.dobuf_size = dobuf_size;
	
_EXIT_:
	return ret;
}

