
CC = gcc

MAJOR_VERSION = $(shell grep MAJOR_VERSION ../include/collections.h | awk '{print $$4}')
MINOR_VERSION = $(shell grep MINOR_VERSION ../include/collections.h | awk '{print $$4}')
BUILD = $(shell grep BUILD ../include/collections.h | awk '{print $$4}')

USR_DIR = /usr/local/lib
LIBNAME = libcollections.so
SONAME = $(LIBNAME).$(MAJOR_VERSION)
DEST_LIBNAME = $(SONAME).$(MINOR_VERSION).$(BUILD)

TARGET = ../bin/$(SONAME)
TARGET_DEST = ../bin/$(DEST_LIBNAME)

INCLUDEDIR = -I../include

LIBDIR = -L/usr/local/lib
LIBS = -lpthread -lm -lrt

CFLAGS = -Wall -Wextra -fPIC -ggdb -O0 -g3 -fvisibility=hidden \
        -DLIBCOLLECTIONS_COMPILE -D_GNU_SOURCE -DLINUX $(INCLUDEDIR)

CHAT_PATH = ./chat
VPATH = ../include:.:$(CHAT_PATH)

CHAT_OBJS =	\
	chat_misc.o		\
	drv_raw_socket.o	\
	ipc_tcp.o		\
	ipc_udp.o

PLUGIN_OBJS =

OBJS =	\
	$(CHAT_OBJS)	\
	$(PLUGIN_OBJS)	\
	cfg.o		\
	chat.o		\
	datetime.o	\
	dll.o		\
	error.o		\
	event.o		\
	file.o		\
	io.o		\
	json.o		\
	mem.o		\
	plugin.o	\
	process.o	\
	random.o	\
	ref.o		\
	specs.o		\
	string.o	\
	stringlist.o	\
	thread.o	\
	timer.o		\
	value.o

$(TARGET): $(OBJS)
	rm -f ../bin/$(LIBNAME)*
	$(CC) -shared -Wl,-soname,$(SONAME) -o $(TARGET_DEST) $(OBJS) $(LIBDIR) $(LIBS)

clean:
	rm -f ../bin/$(LIBNAME)*
	rm -rf $(OBJS) $(TARGET) $(TARGET_DEST) *~ ../include/*~

dest_clean:
	rm -f $(USR_DIR)/$(LIBNAME)*

purge: clean $(TARGET)

install:
	cp -f $(TARGET_DEST) $(USR_DIR)
	rm -rf $(USR_DIR)/$(LIBNAME) $(USR_DIR)/$(SONAME)
	ln -s $(USR_DIR)/$(DEST_LIBNAME) $(USR_DIR)/$(LIBNAME)
	ln -s $(USR_DIR)/$(DEST_LIBNAME) $(USR_DIR)/$(SONAME)

install_headers:
	$(shell [ ! -d /usr/local/include/cl ] && mkdir -p /usr/local/include/cl)
	$(shell for f in `ls ../include/cl | egrep -v '^cl_internal.h'`; do cp -f ../include/cl/$$f /usr/local/include/cl; done)
	cp -f ../include/collections.h /usr/local/include

dev_install: install_headers install

uninstall: dest_clean
	rm -rf /usr/local/include/cl
	rm -f /usr/local/include/collections.h

