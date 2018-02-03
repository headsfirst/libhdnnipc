/* Libhdnnipc - Library for Neural Network Inter-Process Communication
 * Copyright 2018+  Heads First, Janroel Koppen
 *
 */


#include "hdnnipc_Client.h"
#include "read_line.h" // for: readLine

#include <string.h> // for: calloc
#include <stdlib.h> // for: realloc, free
#include <unistd.h> // for: close
#include <sys/socket.h> // for: socket, connect, struct sockaddr
#include <sys/un.h> // for: struct sockaddr_un
#include <stdio.h> // for: snprintf
	
static int _hdnnipc_Client_resize(hdnnipc_Client* me, size_t cbuf_size, size_t dibuf_size, size_t dobuf_size);
static int _writeChunk(int fd, const char* buf, size_t buf_size);
static int _readChunk(int fd, char* buf, size_t buf_size);


hdnnipc_Client* hdnnipc_Client_init(hdnnipc_Client* me, unsigned int ctype, const char* path, const char* preamble) {
	struct sockaddr_un addr_unix;

	if (path == NULL) { goto _FAIL_; } // bail out

	if (me == NULL) {
		if ((me = calloc(1, sizeof(hdnnipc_Client))) == NULL) { goto _FAIL_; }
		// members
		me->ctype = 0;
		me->path = NULL;
		me->preamble = NULL;
		me->cbuf = NULL;
		me->cbuf_size = 0;
		me->dibuf = NULL;
		me->dibuf_size = 0;
		me->dobuf = NULL;
		me->dobuf_size = 0;
		me->fd = -1;
	}
	
	// connect
	me->ctype = ctype;
	free(me->path); me->path = (path != NULL) ? strdup(path) : NULL;
	if (me->fd >= 0) { close(me->fd); me->fd = -1; }
	switch (me->ctype) {
		default:
		case 0x554e4958: // UNIX
		if (me->path != NULL) {
			if ((me->fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) { goto _FAIL_; }
			memset(&addr_unix, 0, sizeof(addr_unix));
			addr_unix.sun_family = AF_UNIX;
			strncpy(addr_unix.sun_path, me->path, sizeof(addr_unix.sun_path)-1);
			if (connect(me->fd, (struct sockaddr*)&addr_unix, sizeof(addr_unix)) < 0) { goto _FAIL_; }
		}
		break;
	}

	// preamble
	free(me->preamble); me->preamble = (preamble != NULL) ? strdup(preamble) : NULL;
	if (_writeChunk(me->fd, me->preamble, strlen(me->preamble)) < strlen(me->preamble)) { goto _FAIL_; }
	
	return me;
_FAIL_:
	return hdnnipc_Client_exit(me);
}

hdnnipc_Client* hdnnipc_Client_exit(hdnnipc_Client* me) {
	if (me != NULL) {
		// members
		if (me->fd >= 0) { close(me->fd); me->fd = -1; }
		free(me->path); me->path = NULL;

		free(me); me = NULL;
	}
	
	return me;
}

hdnnipc_Client* hdnnipc_Client_copy(hdnnipc_Client* me, hdnnipc_Client* copy) {
	if (me == NULL) { goto _FAIL_; } // bail out
	if (me == copy) return copy; // bail out

	// (re-)init
	if ((copy = hdnnipc_Client_init(copy, me->ctype, me->path, me->preamble)) == NULL) { goto _FAIL_; }

	// members
	if (_hdnnipc_Client_resize(copy, me->cbuf_size, me->dibuf_size, me->dobuf_size) < 0) { goto _FAIL_; }

	return copy;
_FAIL_:
	return hdnnipc_Client_exit(copy);
}

int hdnnipc_Client_setCtrlBufferSize(hdnnipc_Client* me, size_t size) {
	return (me != NULL) ? _hdnnipc_Client_resize(me, size, me->dibuf_size, me->dobuf_size) : -1;
}

int hdnnipc_Client_setDataInBufferSize(hdnnipc_Client* me, size_t size) {
	if (me == NULL) return -1; // bail out

	return _hdnnipc_Client_resize(me, me->cbuf_size, size, me->dobuf_size);
}

int hdnnipc_Client_setDataOutBufferSize(hdnnipc_Client* me, size_t size) {
	if (me == NULL) return -1; // bail out

	return _hdnnipc_Client_resize(me, me->cbuf_size, me->dibuf_size, size);
}

char* hdnnipc_Client_getCtrlBuffer(hdnnipc_Client* me) {
	return (me != NULL) ? me->cbuf : NULL;
}

char* hdnnipc_Client_getDataInBuffer(hdnnipc_Client* me) {
	return (me != NULL) ? me->dibuf : NULL;
}

char* hdnnipc_Client_getDataOutBuffer(hdnnipc_Client* me) {
	return (me != NULL) ? me->dobuf : NULL;
}

int hdnnipc_Client_send(hdnnipc_Client* me) {
	int ret = 0;

	if (me == NULL) return -1; // bail out
	
	// ctrl chunk
	if (_writeChunk(me->fd, me->cbuf, me->cbuf_size) < me->cbuf_size) { ret = -3; goto _EXIT_; }
	
	// data chunk
	if (_writeChunk(me->fd, me->dobuf, me->dobuf_size) < me->dobuf_size) { ret = -3; goto _EXIT_; }
	
_EXIT_:
	return ret;
}

int hdnnipc_Client_recv(hdnnipc_Client* me) {
	int ret = 0;

	if (me == NULL) return -1; // bail out

	// ctrl chunk
	if (_readChunk(me->fd, me->cbuf, me->cbuf_size) < me->cbuf_size) { ret = -3; goto _EXIT_; }
	
	// data chunk
	if (_readChunk(me->fd, me->dibuf, me->dibuf_size) < me->dibuf_size) { ret = -3; goto _EXIT_; }
	
_EXIT_:
	return ret;
}

static int _hdnnipc_Client_resize(hdnnipc_Client* me, size_t cbuf_size, size_t dibuf_size, size_t dobuf_size) {
	int ret = 0;
	void* tmp = NULL;

	if (me == NULL) return -1; // bail out

	// ctrl buffer
	if ((tmp = realloc(me->cbuf, cbuf_size * sizeof(char))) == NULL && cbuf_size > 0) { ret = -2; goto _EXIT_; }
	me->cbuf = (char*)tmp;
	me->cbuf_size = cbuf_size;
	
	// data buffers
	if ((tmp = realloc(me->dibuf, dibuf_size * sizeof(char))) == NULL && dibuf_size > 0) { ret = -2; goto _EXIT_; }
	me->dibuf = (char*)tmp;
	me->dibuf_size = dibuf_size;
	if ((tmp = realloc(me->dobuf, dobuf_size * sizeof(char))) == NULL && dobuf_size > 0) { ret = -2; goto _EXIT_; }
	me->dobuf = (char*)tmp;
	me->dobuf_size = dobuf_size;
	
_EXIT_:
	return ret;
}

static int _writeChunk(int fd, const char* buf, size_t buf_size) {
	int ret = 0;
	char* size_str = NULL;

	if (fd < 0) return -1; // bail out

	if ((size_str = (char*) calloc(32, sizeof(char))) == NULL) { ret = -2; goto _EXIT_; } // OPTIMIZE ME: heap?
	// size
	snprintf(size_str, 32, "%llx\n", (long long)((buf != NULL) ? buf_size : 0));
	if (write(fd, size_str, strlen(size_str)) < strlen(size_str)) { ret = -3; goto _EXIT_; }
	// data
	if (buf != NULL) {
		if ((ret = write(fd, buf, buf_size)) < buf_size) { goto _EXIT_; }
	}

_EXIT_:
	free(size_str);
	return ret;
}

static int _readChunk(int fd, char* buf, size_t buf_size) {
	int ret = 0;
	char* size_str = NULL;
	size_t size = 0;
	size_t size_x = 0;
	void* xcs = NULL;
	size_t xcs_size = 0;

	if (fd < 0) return -1; // bail out

	if ((size_str = (char*) calloc(32, sizeof(char))) == NULL) { ret = -2; goto _EXIT_; } // OPTIMIZE ME: heap?
	// size
	if (readLine(fd, size_str, 32) < 0) { ret = -3; goto _EXIT_; }
	size = (size_t)strtoull(size_str, NULL, 16); // strip \n first?
	if (size > buf_size) {
		size_x = size - buf_size;
		size = buf_size;
	}
	// data
	if (size > 0 && buf != NULL) {
		if ((ret = read(fd, (void*)buf, size)) < size) { goto _EXIT_; }
	}
	if (size_x > 0) {
		if ((xcs = malloc(4096)) == NULL) { ret = -2; goto _EXIT_; }
		xcs_size = 4096;
		while (size_x > 0) {
			size = (size_x < xcs_size) ? size_x : xcs_size; // minimum
			if (read(fd, xcs, size) < size) { goto _EXIT_; }
			size_x -= size;
		}
	}

_EXIT_:
	free(xcs);
	free(size_str);
	return ret;
}
