
.PHONY: outputdir clean dest_clean purge install install_headers dev_install \
    uninstall

CC = gcc

GCCVERSION = 4
ARCH_TEST := $(shell uname -m)
GCCVERSION_TEST := $(shell expr `gcc -dumpversion | cut -f1 -d.` \> 4)
OS := $(shell uname -o | tr '[:lower:]' '[:upper:]' | tr '/' '_')

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
LIBDIR = -L/usr/local/lib

CHAT_PATH = ./chat
PLUGIN_PATH = ./plugin
JNI_PATH = ./plugin/java
IMAGE_PATH = ./image
ADT_PATH = ./adt

ifeq ($(CL_IMAGEAPI), 1)
    OPENCV_LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui \
		  -lfreetype -ljpeg -lpng -ltiff -lswscale -ljasper

    IMAGES = -DCL_USE_IMAGEAPI
    INCLUDEDIR += -I/usr/include/freetype2
    IMAGE_SOURCES := $(wildcard $(IMAGE_PATH)/*.c)
    IMAGE_OBJS = $(IMAGE_SOURCES:.c=.o)
endif

# Is the python plugin enabled?
ifeq ($(PYPLUGIN), 1)
    PYTHON_LIBS = -lpython2.7
    PLUGIN_PYTHON = -DPLUGIN_PYTHON
    INCLUDEDIR += -I/usr/include/python2.7
    PY_DL_OBJS =	\
					$(PLUGIN_PATH)/dl_python.o
endif

# Is the java plugin enabled?
ifeq ($(JAVAPLUGIN), 1)
    JAVA_LIBS = -ljvm
    PLUGIN_JAVA = -DPLUGIN_JAVA
    INCLUDEDIR += -I/usr/lib/jvm/default-java/include
    INCLUDEDIR += -I/usr/lib/jvm/default-java/include/linux
    LIBDIR += -L/usr/lib/jvm/default-java/jre/lib/$(ARCH)/server/
    JAVA_DL_OBJS =	\
					$(PLUGIN_PATH)/dl_java.o
endif

PLUGIN_LIBS = $(PYTHON_LIBS) $(JAVA_LIBS)
LIBS = -lpthread -lm -lrt -ldl -lmagic $(PLUGIN_LIBS) $(OPENCV_LIBS)

ifneq ($(OS), GNU_LINUX)
    LIBS += -lintl
endif

# Elf plugin is always enabled
PLUGIN_ELF = -DPLUGIN_ELF
PLUGINS = $(PLUGIN_ELF) $(PLUGIN_PYTHON) $(PLUGIN_JAVA)
CFLAGS = -Wall -Wextra -fPIC -ggdb -O0 -g3 -DLIBCOLLECTIONS_COMPILE \
		 -D_GNU_SOURCE -D$(OS) $(PLUGINS) $(IMAGES) $(INCLUDEDIR)

ifeq ($(GCCVERSION), 5)
    CFLAGS += -fgnu89-inline
endif

CHAT_SOURCES := $(wildcard $(CHAT_PATH)/*.c)
CHAT_OBJS = $(CHAT_SOURCES:.c=.o)

DL_OBJS = \
		  $(PLUGIN_PATH)/dl_elf.o	\
		  $(PY_DL_OBJS) 			\
		  $(JAVA_DL_OBJS)

DL_SOURCES := $(wildcard $(PLUGIN_PATH)/dl_*.c)
PLUGIN_SOURCES := $(wildcard $(PLUGIN_PATH)/*.c)
PLUGIN_SOURCES := $(filter-out $(DL_SOURCES), $(PLUGIN_SOURCES))
PLUGIN_OBJS = \
			  $(PLUGIN_SOURCES:.c=.o)	\
			  $(DL_OBJS)

ADT_SOURCES := $(wildcard $(ADT_PATH)/*.c)
SOURCES := $(wildcard *.c)
OBJS =	\
		$(CHAT_OBJS)			\
		$(PLUGIN_OBJS)			\
		$(IMAGE_OBJS)			\
		$(ADT_SOURCES:.c=.o)	\
		$(SOURCES:.c=.o)

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

