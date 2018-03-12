// A little Go plugin example
package main

import "C"
import (
	"encoding/json"
	"fmt"
	"unsafe"

	"collections/pkg/collections"
)

//
// Mandatory plugin functions
//

//export plugin_name
func plugin_name() *C.char {
	fmt.Println("Ola")
	return C.CString("Go-Plugin")
}

//export plugin_version
func plugin_version() *C.char {
	return C.CString("0.1")
}

//export plugin_author
func plugin_author() *C.char {
	return C.CString("Rodrigo Freitas")
}

//export plugin_description
func plugin_description() *C.char {
	return C.CString("Go plugin example")
}

//
// Startup and shutdown
//

//export plugin_init
func plugin_init() int {
	fmt.Println("Here we are, at the beginning")
	return 0
}

//export plugin_uninit
func plugin_uninit() {
	fmt.Println("And at the end...")
}

//
// Plugin functions
//

//export foo_int
func foo_int() int {
	return 42
}

//export foo_uint
func foo_uint() uint {
	return 420
}

//export foo_char
func foo_char() byte {
	return 'b'
}

//export foo_uchar
func foo_uchar() byte {
	return 230
}

//export foo_sint
func foo_sint() int16 {
	return 421
}

//export foo_usint
func foo_usint() uint16 {
	return 4201
}

//export foo_float
func foo_float() float32 {
	return 42.5
}

//export foo_double
func foo_double() float64 {
	return 4.2
}

//export foo_long
func foo_long() int32 {
	return 42000
}

//export foo_ulong
func foo_ulong() uint32 {
	return 420001
}

//export foo_llong
func foo_llong() int64 {
	return 420009
}

//export foo_ullong
func foo_ullong() uint64 {
	return 4200019
}

//export foo_boolean
func foo_boolean() bool {
	return true
}

//export foo_args
func foo_args(args *C.char) {
	var dat map[string]interface{}
	s := C.GoString(args)

	if err := json.Unmarshal([]byte(s), &dat); err != nil {
		fmt.Println("Json parsing error")
		return
	}

	fmt.Println(dat)
}

//export foo_pointer
func foo_pointer(args unsafe.Pointer) {
	fmt.Printf("Addres is %p\n", args)
	fmt.Println("We're here...")
}

//export another_outside_api
func another_outside_api(args unsafe.Pointer) int {
	arguments, err := collections.LoadArguments(args)

	if err != nil {
		fmt.Println(err)
		return -1
	}

	arg1 := arguments.Int("arg1")
	fmt.Println(arg1)

	return 412
}

func main() {
	//
	// We need the main function otherwise the ELF shared object created will be
	// in the wrong format, as an AR (archive) file and not an ELF shared object.
	//
}
