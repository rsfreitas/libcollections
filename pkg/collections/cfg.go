//
// Description: Functions to handle cl_cfg_file_t objects from within Go code.
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

// ClConfigFile is the Go version of cl_cfg_file_t object.
type ClConfigFile struct {
	data unsafe.Pointer
}

// Get "reads" the value of an entry inside a configuration block and returns
// it as a string.
func (c *ClConfigFile) Get(block, entry string) string {
	cBlock := C.CString(block)
	defer C.free(unsafe.Pointer(cBlock))

	cEntry := C.CString(entry)
	defer C.free(unsafe.Pointer(cEntry))

	obj := C.cl_cfg_get_value(c.data, cBlock, cEntry)
	defer C.cl_object_unref(unsafe.Pointer(obj))

	cObject := NewObject(obj)
	return cObject.Get()
}

// Set updates the value of an entry inside a configuration block.
func (c *ClConfigFile) Set(block, entry, content string) {
	cBlock := C.CString(block)
	defer C.free(unsafe.Pointer(cBlock))

	cEntry := C.CString(entry)
	defer C.free(unsafe.Pointer(cEntry))

	cContent := C.CString(content)
	defer C.free(unsafe.Pointer(cContent))

	C.cl_cfg_set_value_ex(c.data, cBlock, cEntry, cContent)
}

// NewConfigFile gives a new ClConfigFile structure based on a content @data
// previously received.
func NewConfigFile(data unsafe.Pointer) *ClConfigFile {
	return &ClConfigFile{
		data: data,
	}
}
