
.PHONY: outputdir clean dest_clean purge install install_headers dev_install \
    uninstall

CC = gcc
GCCVERSION = 4
ARCH_TEST := $(shell uname -m)
GCCVERSION_TEST := $(shell expr `gcc -dumpversion | cut -f1 -d.` \>= 4)

ifeq ($(ARCH_TEST), x86_64)
    ARCH = amd64
else
    ARCH = i386
endif

ifeq ($(GCCVERSION_TEST), 1)
    GCCVERSION = 5
endif

MAJOR_VERSION := $(shell grep MAJOR_VERSION ../include/collections.h | awk '{print $$4}')
MINOR_VERSION := $(shell grep MINOR_VERSION ../include/collections.h | awk '{print $$4}')
BUILD := $(shell grep BUILD ../include/collections.h | awk '{print $$4}')

USR_DIR = /usr/local/lib
PREFIX = libcollections
LIBNAME = $(PREFIX).so
SONAME = $(LIBNAME)
DEST_LIBNAME = $(LIBNAME).$(MAJOR_VERSION).$(MINOR_VERSION).$(BUILD)

TARGET_DIR = ../bin
TARGET = $(TARGET_DIR)/$(SONAME)
TARGET_DEST = $(TARGET_DIR)/$(DEST_LIBNAME)

INCLUDEDIR = -I../include
INCLUDEDIR += -I/usr/include/python2.7
INCLUDEDIR += -I/usr/lib/jvm/default-java/include
INCLUDEDIR += -I/usr/lib/jvm/default-java/include/linux

LIBDIR = -L/usr/local/lib
LIBDIR += -L/usr/lib/jvm/default-java/jre/lib/$(ARCH)/server/

#ifeq ($(ARCH), i386)
#    OPENCV_LIBS = /usr/lib/libopencv_highgui.a
#else
#    OPENCV_LIBS = -lopencv_highgui
#endif

PYTHON_LIBS = -lpython2.7
#JAVA_LIBS = -ljvm
PLUGIN_LIBS = $(PYTHON_LIBS) $(JAVA_LIBS)

LIBS = -lpthread -lm -lrt -ldl -lmagic $(PLUGIN_LIBS) $(OPENCV_LIBS)

# TODO: Fix here
PLUGIN_C = -DPLUGIN_C
#PLUGIN_PYTHON = -DPLUGIN_PYTHON
#PLUGIN_JAVA = -DPLUGIN_JAVA

PLUGINS = $(PLUGIN_C) $(PLUGIN_PYTHON) $(PLUGIN_JAVA)
CFLAGS = -Wall -Wextra -fPIC -ggdb -O0 -g3 -DLIBCOLLECTIONS_COMPILE \
		 -D_GNU_SOURCE -DLINUX $(PLUGINS) $(INCLUDEDIR)

ifeq ($(GCCVERSION), 5)
    CFLAGS += -fgnu89-inline
endif

CHAT_PATH = ./chat
PLUGIN_PATH = ./plugin
JNI_PATH = ./plugin/java
VPATH = ../include:.:$(CHAT_PATH):$(PLUGIN_PATH):$(JNI_PATH)

CHAT_OBJS =	\
	chat_misc.o			\
	drv_raw_socket.o		\
	ipc_tcp.o			\
	ipc_udp.o

# TODO: Fix here
#JNI_OBJS =		\
	jni_CpluginArguments.o		\
	jni_CpluginObject.o		\
	jni_CpluginType.o

#	dl_python.o		\
#	dl_java.o		\

PLUGIN_OBJS =	\
	api_parser.o		\
	call.o			\
	dl.o			\
	dl_c.o			\
	info.o			\
	pl_misc.o		\
	rv.o			\
	$(JNI_OBJS)

OBJS =	\
	$(CHAT_OBJS)		\
	$(PLUGIN_OBJS)		\
	cfg.o			\
	chat.o			\
	counter.o		\
	datetime.o		\
	dll.o			\
	error.o			\
	event.o			\
	file.o			\
	glist.o			\
	init.o			\
	io.o			\
	json.o			\
	list.o			\
	log.o			\
	mem.o			\
	object.o		\
	plugin.o		\
	process.o		\
	random.o		\
	ref.o			\
	specs.o			\
	string.o		\
	stringlist.o		\
	thread.o		\
	timeout.o		\
	timer.o			\
	typeof.o		\
	util.o

$(TARGET): outputdir $(OBJS)
	$(CC) -shared -Wl,-soname,$(SONAME),--version-script,$(PREFIX).sym -o $(TARGET_DEST) $(OBJS) $(LIBDIR) $(LIBS)

clean:
	rm -f ../bin/$(LIBNAME)*
	rm -rf $(OBJS) $(TARGET) $(TARGET_DEST) *~ ../include/*~ ../bin/*.so

dest_clean:
	rm -f $(USR_DIR)/$(LIBNAME)*

purge: clean $(TARGET)

install:
	cp -f $(TARGET_DEST) $(USR_DIR)
	rm -rf $(USR_DIR)/$(LIBNAME) $(USR_DIR)/$(SONAME)
	ln -sf $(USR_DIR)/$(DEST_LIBNAME) $(USR_DIR)/$(LIBNAME)
	ln -sf $(USR_DIR)/$(DEST_LIBNAME) $(USR_DIR)/$(SONAME)

install_headers:
	$(shell [ ! -d /usr/local/include/cl ] && mkdir -p /usr/local/include/cl)
	$(shell for f in `ls ../include/cl | egrep -v '^cl_internal.h|^cl_typeof.h'`; do cp -f ../include/cl/$$f /usr/local/include/cl; done)
	cp -f ../include/collections.h /usr/local/include

dev_install: install_headers install

uninstall: dest_clean
	rm -rf /usr/local/include/cl
	rm -f /usr/local/include/collections.h

outputdir: $(TARGET_DIR)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

