//
// Description: Functions to handle cl_string_t objects from within Go code.
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

// ClString is the Go version of a cl_string_t object.
type ClString struct {
	data unsafe.Pointer
}

// Get retrieves current cl_string_t value and returns it.
func (s *ClString) Get() string {
	return C.GoString(C.cl_string_valueof(s.data))
}

// Set updates the internal cl_string_t value with @content.
func (s *ClString) Set(content string) {
	cs := C.CString(content)
	defer C.free(unsafe.Pointer(cs))

	C.cl_string_set_content(s.data, cs)
}

// NewString gives a new ClString structure based on a content @data previously
// received.
func NewString(data unsafe.Pointer) *ClString {
	return &ClString{
		data: data,
	}
}
