# libcollections

A C library with some generic collections, strings and a few more...

## License

LGPLv2

## Overview

This library is intended to provide features to C applications and
ease their development.

### Features

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

## Compiling

In order to compile the project you must have at least the following programs
installed;

* C compiler
* CMake, at least version 2.8

After installing them you can compile the project using the commands:

* mkdir build
* cd build
* cmake ..
* make

Some options are available to enable or disable some features:

 * IMAGE: to enable/disable image API.
 * PYPLUGIN: to enable/disable Python plugin support.
 * JAVAPLUGIN: to enable/disable Java plugin support.

* If one is using a gcc 5 compiler, the linked target must use the option
**-fgnu89-inline**.

* If the image API needs to be used by a dependency the **CL\_USE\_IMAGEAPI**
must be used while including the library's main header.

