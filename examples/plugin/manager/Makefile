
CC = gcc
TARGET = plugin-manager

INCLUDEDIR = -I../../../include
CFLAGS = -Wall -O0 -ggdb -D_GNU_SOURCE -fgnu89-inline $(INCLUDEDIR)

LIBDIR = -L/usr/local/lib
LIBS = -lcollections

OBJECTS =	\
	example.o

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBDIR) $(LIBS)

clean:
	rm -rf $(OBJECTS) $(TARGET)

