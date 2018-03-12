//
// Description: Functions to load (plugin public) function arguments.
// Author: Rodrigo Freitas
// Created at: Seg Mar 12 16:40:39 -03 2018
//
package collections

/*
#cgo CFLAGS: -I/usr/local/include/collections -fgnu89-inline
#cgo LDFLAGS: -L/usr/local/lib -lcollections
#include <collections.h>
#include <stdlib.h>
*/
import "C"
import (
	"unsafe"
)

// ClPluginArguments stores every information required to load arguments passed
// to functions.
type ClPluginArguments struct {
	arguments unsafe.Pointer
}

// Int loads an argument of name argumentName and returns it as an int value.
func (a *ClPluginArguments) Int(argumentName string) int {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return int(C.cl_plugin_argument_int(a.arguments, cs))
}

// UInt loads an argument of name argumentName and returns it as an uint value.
func (a *ClPluginArguments) UInt(argumentName string) uint {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return uint(C.cl_plugin_argument_uint(a.arguments, cs))
}

// Byte loads an argument of name argumentName and returns it as an byte value.
func (a *ClPluginArguments) Byte(argumentName string) byte {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return byte(C.cl_plugin_argument_char(a.arguments, cs))
}

// Int16 loads an argument of name argumentName and returns it as an int16 value.
func (a *ClPluginArguments) Int16(argumentName string) int16 {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return int16(C.cl_plugin_argument_sint(a.arguments, cs))
}

// UInt16 loads an argument of name argumentName and returns it as an uint16 value.
func (a *ClPluginArguments) UInt16(argumentName string) uint16 {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return uint16(C.cl_plugin_argument_usint(a.arguments, cs))
}

// Float32 loads an argument of name argumentName and returns it as an float32 value.
func (a *ClPluginArguments) Float32(argumentName string) float32 {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return float32(C.cl_plugin_argument_float(a.arguments, cs))
}

// Float64 loads an argument of name argumentName and returns it as an float64 value.
func (a *ClPluginArguments) Float64(argumentName string) float64 {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return float64(C.cl_plugin_argument_double(a.arguments, cs))
}

// Int32 loads an argument of name argumentName and returns it as an int32 value.
func (a *ClPluginArguments) Int32(argumentName string) int32 {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return int32(C.cl_plugin_argument_long(a.arguments, cs))
}

// UInt32 loads an argument of name argumentName and returns it as an uint32 value.
func (a *ClPluginArguments) UInt32(argumentName string) uint32 {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return uint32(C.cl_plugin_argument_ulong(a.arguments, cs))
}

// Int64 loads an argument of name argumentName and returns it as an int64 value.
func (a *ClPluginArguments) Int64(argumentName string) int64 {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return int64(C.cl_plugin_argument_llong(a.arguments, cs))
}

// UInt64 loads an argument of name argumentName and returns it as an uint64 value.
func (a *ClPluginArguments) UInt64(argumentName string) uint64 {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return uint64(C.cl_plugin_argument_ullong(a.arguments, cs))
}

// Bool loads an argument of name argumentName and returns it as an bool value.
func (a *ClPluginArguments) Bool(argumentName string) bool {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	return bool(C.cl_plugin_argument_bool(a.arguments, cs))
}

// Pointer loads an argument of name argumentName and returns it as an unsage.Pointer value.
func (a *ClPluginArguments) Pointer(argumentName string) unsafe.Pointer {
	cs := C.CString(argumentName)
	defer C.free(unsafe.Pointer(cs))

	var ptr unsafe.Pointer
	_, err := C.cl_plugin_argument_pointer(a.arguments, cs, &ptr)

	//FIXME: Correctly check error here...
	if err != nil {
		return nil
	}

	return ptr
}

// LoadsArguments is responsible to load the arguments received by a plugin
// public function and returns a ClPluginArguments type to allow specific
// argument extraction.
func LoadArguments(arguments unsafe.Pointer) (*ClPluginArguments, error) {
	return &ClPluginArguments{
		arguments: unsafe.Pointer(arguments),
	}, nil
}
