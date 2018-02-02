# -*- makefile -*-

AUTOMAKE_OPTIONS = subdir-objects

#lib_LTLIBRARIES		= libhdnnipc.la
pkglib_LTLIBRARIES		= libhdnnipc.la


libhdnnipc_la_LDFLAGS	= \
	-version-info 0:1:0


libhdnnipc_la_SOURCES = \
	hdnnipc_Client.c \
	hdnnipc_Server.c \
	read_line.c

include_HEADERS = \
	hdnnipc.h \
	hdnnipc_Client.h \
	hdnnipc_Server.h \
	read_line.h


AM_CPPFLAGS     =
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	-I../

CLEANFILES      = *~


DISTCLEANFILES  = .deps/*.P
