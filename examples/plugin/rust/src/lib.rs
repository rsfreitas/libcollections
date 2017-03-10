
extern crate libc;
extern crate rcollections;

use rcollections::utils;
use libc::c_char;
use std::ffi::CString;

/**
 *
 * Plugin informations...
 *
 */
#[no_mangle]
//pub extern "C" fn plugin_name() -> *const u8 {
pub extern "C" fn plugin_name() -> *mut c_char {
    let s = CString::new("RUST-PLUGIN").unwrap();
    s.into_raw()
//    "RUST-PLUGIN\0".as_ptr()
}

#[no_mangle]
//pub extern "C" fn plugin_version() -> *const u8 {
pub extern "C" fn plugin_version() -> *mut c_char {
    let s = CString::new("0.1").unwrap();
    s.into_raw()
//    "0.1\0".as_ptr()
}

#[no_mangle]
//pub extern "C" fn plugin_author() -> *const u8 {
pub extern "C" fn plugin_author() -> *mut c_char {
    let s = CString::new("Rodrigo Freitas").unwrap();
    s.into_raw()
//    "Rodrigo Freitas\0".as_ptr()
}

#[no_mangle]
//pub extern "C" fn plugin_api() -> *const u8 {
pub extern "C" fn plugin_api() -> *mut c_char {
    let s = CString::new("{\"API\": [\
        { \"name\": \"foo_int\", \"return_type\": \"int\" },\
        { \"name\": \"foo_uint\", \"return_type\": \"uint\" },\
        { \"name\": \"foo_char\", \"return_type\": \"char\" },\
        { \"name\": \"foo_uchar\", \"return_type\": \"uchar\" },\
        { \"name\": \"foo_sint\", \"return_type\": \"sint\" },\
        { \"name\": \"foo_usint\", \"return_type\": \"usint\" },\
        { \"name\": \"foo_float\", \"return_type\": \"float\" },\
        { \"name\": \"foo_double\", \"return_type\": \"double\" },\
        { \"name\": \"foo_long\", \"return_type\": \"long\" },\
        { \"name\": \"foo_ulong\", \"return_type\": \"ulong\" },\
        { \"name\": \"foo_llong\", \"return_type\": \"llong\" },\
        { \"name\": \"foo_ullong\", \"return_type\": \"ullong\" },\
        { \"name\": \"foo_boolean\", \"return_type\": \"boolean\" },\
        { \"name\": \"foo_args\", \"return_type\": \"void\", \"arguments\": [\
            { \"name\": \"arg1\", \"type\": \"int\" },\
            { \"name\": \"arg2\", \"type\": \"uint\" },\
            { \"name\": \"arg3\", \"type\": \"sint\" },\
            { \"name\": \"arg4\", \"type\": \"usint\" },\
            { \"name\": \"arg5\", \"type\": \"char\" },\
            { \"name\": \"arg6\", \"type\": \"uchar\" },\
            { \"name\": \"arg7\", \"type\": \"float\" },\
            { \"name\": \"arg8\", \"type\": \"double\" },\
            { \"name\": \"arg9\", \"type\": \"long\" },\
            { \"name\": \"arg10\", \"type\": \"ulong\" },\
            { \"name\": \"arg11\", \"type\": \"llong\" },\
            { \"name\": \"arg12\", \"type\": \"ullong\" },\
            { \"name\": \"arg13\", \"type\": \"boolean\" },\
            { \"name\": \"arg14\", \"type\": \"string\" }\
            ] }\
    ]\
    }").unwrap();
    s.into_raw()
/*    "{\"API\": [\
        { \"name\": \"foo_int\", \"return_type\": \"int\" },\
        { \"name\": \"foo_uint\", \"return_type\": \"uint\" },\
        { \"name\": \"foo_char\", \"return_type\": \"char\" },\
        { \"name\": \"foo_uchar\", \"return_type\": \"uchar\" },\
        { \"name\": \"foo_sint\", \"return_type\": \"sint\" },\
        { \"name\": \"foo_usint\", \"return_type\": \"usint\" },\
        { \"name\": \"foo_float\", \"return_type\": \"float\" },\
        { \"name\": \"foo_double\", \"return_type\": \"double\" },\
        { \"name\": \"foo_long\", \"return_type\": \"long\" },\
        { \"name\": \"foo_ulong\", \"return_type\": \"ulong\" },\
        { \"name\": \"foo_llong\", \"return_type\": \"llong\" },\
        { \"name\": \"foo_ullong\", \"return_type\": \"ullong\" },\
        { \"name\": \"foo_boolean\", \"return_type\": \"boolean\" },\
        { \"name\": \"foo_args\", \"return_type\": \"void\", \"arguments\": [\
            { \"name\": \"arg1\", \"type\": \"int\" },\
            { \"name\": \"arg2\", \"type\": \"uint\" },\
            { \"name\": \"arg3\", \"type\": \"sint\" },\
            { \"name\": \"arg4\", \"type\": \"usint\" },\
            { \"name\": \"arg5\", \"type\": \"char\" },\
            { \"name\": \"arg6\", \"type\": \"uchar\" },\
            { \"name\": \"arg7\", \"type\": \"float\" },\
            { \"name\": \"arg8\", \"type\": \"double\" },\
            { \"name\": \"arg9\", \"type\": \"long\" },\
            { \"name\": \"arg10\", \"type\": \"ulong\" },\
            { \"name\": \"arg11\", \"type\": \"llong\" },\
            { \"name\": \"arg12\", \"type\": \"ullong\" },\
            { \"name\": \"arg13\", \"type\": \"boolean\" },\
            { \"name\": \"arg14\", \"type\": \"string\" }\
            ] }\
    ]\
    }\0".as_ptr()*/
}

#[no_mangle]
//pub extern "C" fn plugin_description() -> *const u8 {
pub extern "C" fn plugin_description() -> *mut c_char {
//    "Rust plugin example\0".as_ptr()
    let s = CString::new("Rust plugin example").unwrap();
    s.into_raw()
}

/**
 *
 * Init and uninit function
 *
 */
#[no_mangle]
pub extern "C" fn plugin_init() -> i32 {
    println!("RUST init");
    0 /* OK */
}

#[no_mangle]
pub extern "C" fn plugin_uninit() {
    println!("RUST uninit");
}

/**
 *
 * Main API
 *
 */
#[no_mangle]
pub extern "C" fn foo_int() -> i32 {
    42
}

#[no_mangle]
pub extern "C" fn foo_uint() -> u32 {
    420
}

#[no_mangle]
pub extern "C" fn foo_sint() -> i16 {
    421
}

#[no_mangle]
pub extern "C" fn foo_usint() -> u16 {
    4201
}

#[no_mangle]
pub extern "C" fn foo_char() -> i8 {
    'a' as i8
}

#[no_mangle]
pub extern "C" fn foo_uchar() -> u8 {
    230
}

#[no_mangle]
pub extern "C" fn foo_float() -> f32 {
    42.5
}

#[no_mangle]
pub extern "C" fn foo_double() -> f64 {
    4.2
}

#[no_mangle]
pub extern "C" fn foo_boolean() -> bool {
    false
}

#[no_mangle]
pub extern "C" fn foo_long() -> i32 {
    42000
}

#[no_mangle]
pub extern "C" fn foo_ulong() -> u32 {
    420001
}

#[no_mangle]
pub extern "C" fn foo_llong() -> i64 {
    420009
}

#[no_mangle]
pub extern "C" fn foo_ullong() -> u64 {
    4200019
}

#[no_mangle]
pub extern "C" fn foo_args(args: *const i8) {
    let jargs = match utils::retrieve_c_arguments_as_json(args) {
        Ok(value) => value,
        Err(_) => return,
    };

//    println!("{:?}", jargs);
//    println!("{}", jargs);

    let arg1 = jargs.as_object().unwrap().get("arg1").unwrap().as_u64().unwrap();
//    println!("{}", arg1);
}

