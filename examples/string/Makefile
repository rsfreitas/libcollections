
CC = gcc
TARGET = string

INCLUDEDIR = -I../../include
CFLAGS = -Wall -O0 -ggdb -DUSE_CL_IMAGEAPI -fgnu89-inline -D_GNU_SOURCE $(INCLUDEDIR)

LIBDIR = -L/usr/local/lib
LIBS = -lcollections -lopencv_core -lm

OBJECTS =	\
	example.o

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBDIR) $(LIBS)

clean:
	rm -rf $(OBJECTS) $(TARGET)

