// A little Go plugin example
package plugin

import "C"
import (
	"encoding/json"
	"fmt"
)

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

//export plugin_api
func plugin_api() *C.char {
	return C.CString("{ \"API\":[ " +
		" { \"name\": \"foo_int\", \"return_type\": \"int\" }," +
		" { \"name\": \"foo_uint\", \"return_type\": \"uint\" }," +
		" { \"name\": \"foo_char\", \"return_type\": \"char\" }," +
		" { \"name\": \"foo_uchar\", \"return_type\": \"uchar\" }," +
		" { \"name\": \"foo_sint\", \"return_type\": \"sint\" }," +
		" { \"name\": \"foo_usint\", \"return_type\": \"usint\" }," +
		" { \"name\": \"foo_float\", \"return_type\": \"float\" }," +
		" { \"name\": \"foo_double\", \"return_type\": \"double\" }," +
		" { \"name\": \"foo_long\", \"return_type\": \"long\" }," +
		" { \"name\": \"foo_ulong\", \"return_type\": \"ulong\" }," +
		" { \"name\": \"foo_llong\", \"return_type\": \"llong\" }," +
		" { \"name\": \"foo_ullong\", \"return_type\": \"ullong\" }," +
		" { \"name\": \"foo_boolean\", \"return_type\": \"boolean\" }," +
		" { \"name\": \"foo_args\", \"return_type\": \"void\", \"arguments\": [" +
		" { \"name\": \"arg1\", \"type\": \"int\" }," +
		" { \"name\": \"arg2\", \"type\": \"uint\" }," +
		" { \"name\": \"arg3\", \"type\": \"sint\" }," +
		" { \"name\": \"arg4\", \"type\": \"usint\" }," +
		" { \"name\": \"arg5\", \"type\": \"char\" }," +
		" { \"name\": \"arg6\", \"type\": \"uchar\" }," +
		" { \"name\": \"arg7\", \"type\": \"float\" }," +
		" { \"name\": \"arg8\", \"type\": \"double\" }," +
		" { \"name\": \"arg9\", \"type\": \"long\" }," +
		" { \"name\": \"arg10\", \"type\": \"ulong\" }," +
		" { \"name\": \"arg11\", \"type\": \"llong\" }," +
		" { \"name\": \"arg12\", \"type\": \"ullong\" }," +
		" { \"name\": \"arg13\", \"type\": \"boolean\" }," +
		" { \"name\": \"arg14\", \"type\": \"string\" }" +
		" ] } " +
		"]" +
		"}")
}

//export plugin_init
func plugin_init() int {
	fmt.Println("Here we are, at the beginning")
	return 0
}

//export plugin_uninit
func plugin_uninit() {
	fmt.Println("And at the end...")
}

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
	return 'a'
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
