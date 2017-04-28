# libcollections

A C library with some generic collections, strings and a few more...

## License

LGPLv2

## Contents

* INI configuration
* JSON parser
* Datetime functions
* String manipulation (also string lists)
* Doubly linked list
* Thread utils
* Timer functions
* IPC socket protocol
* And some more..

## Dependencies

* Pthreads
* Realtime extensions library
* python-enum34 for python extension
* libmagic
* Java 1.6 JDK
* OpenCv 2 (libtiff, libjpeg, libpng)
* libfreetype
* libswscale

## Important

* To compile the library you run the _make_ command. This will build the library
with plugin support (ELF format only). _sudo make install_ will install the
library in your environment to be used and *sudo make dev\_install* will install
all necessary header files to compile an application with libcollections.

Some options may be used while compiling to turn some features on:

 * CL\_IMAGEAPI=1: to export the image API
 * PYPLUGIN=1: to support Python plugins
 * JAVAPLUGIN=1: to support Java plugins

* If one is using a gcc 5 compiler, the linked target must use the option
**-fgnu89-inline**.

* If the image API needs to be used by a dependency the **USE\_CL\_IMAGEAPI**
must be used while including the library's main header.

