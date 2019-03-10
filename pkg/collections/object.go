//
// Description: Functions to handle cl_object_t objects from within Go code.
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

// ClObject is the Go version of a cl_object_t object.
type ClObject struct {
	data unsafe.Pointer
}

// Get retrieves current cl_object_t value and returns it as a string, independent
// of what is inside of it.
func (o *ClObject) Get() string {
	s := C.cl_object_to_cstring(o.data)
	gValue := NewString(s)

	return gValue.Get()
}

// Set updates the internal cl_object_t value with @content.
func (o *ClObject) Set(content string) {
	cContent := C.CString(content)
	defer C.free(unsafe.Pointer(cContent))

	C.cl_object_set_ex(o.data, cContent)
}

// NewObject gives a new ClObject structure based on a content @data previously
// received.
func NewObject(data unsafe.Pointer) *ClObject {
	return &ClObject{
		data: data,
	}
}
