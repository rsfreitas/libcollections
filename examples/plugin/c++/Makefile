
CC = g++

USR_DIR = /usr/local/lib
PLUGIN_NAME = cpp-plugin.so
SONAME = $(PLUGIN_NAME)

TARGET = $(SONAME)

INCLUDEDIR = -I../../../include

DEVC_FLAGS = -DLINUX
CXXFLAGS = -Wall -Wextra -O0 -fPIC -ggdb -g3 -fvisibility=hidden \
	 $(INCLUDEDIR) -D_GNU_SOURCE

LIBDIR = -L/usr/local/lib
LIBS = -lcollections

OBJS = example.o

$(TARGET): $(OBJS)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $(TARGET) $(OBJS) $(LIBDIR) $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET) *~

purge: clean $(TARGET)

