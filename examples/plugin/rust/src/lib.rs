extern crate libc;

use std::ffi::CStr;
use std::str;

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
    }
}

/**
 *
 * Funções de informaço do plugin
 *
 */
#[no_mangle]
pub extern "C" fn plugin_name() -> *const u8 {
    "rhttp".as_ptr()
}

#[no_mangle]
pub extern "C" fn plugin_version() -> *const u8 {
    "0.1".as_ptr()
}

#[no_mangle]
pub extern "C" fn plugin_author() -> *const u8 {
    "Rodrigo Freitas".as_ptr()
}

/*        { \"name\": \"foo_uint\", \"return_type\": \"uint\" },\
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
        { \"name\": \"foo_boolean\", \"return_type\": \"boolean\" }\*/
#[no_mangle]
pub extern "C" fn plugin_api() -> *const u8 {
    "{\"API\": [\
        { \"name\": \"foo_int\", \"return_type\": \"int\" },\
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
    }".as_ptr()
}

#[no_mangle]
pub extern "C" fn plugin_description() -> *const u8 {
    "Rust HTTP JOB sender".as_ptr()
}

/**
 *
 * Inicialização e finalização do plugin
 *
 */
#[no_mangle]
pub extern "C" fn plugin_init() -> i32 {
    0 /* OK */
}

#[no_mangle]
pub extern "C" fn plugin_uninit() {
    println!("uninit");
}

/**
 *
 * API principal
 *
 */
#[no_mangle]
pub extern "C" fn foo_int() -> i32 {
    42
}

#[no_mangle]
pub extern "C" fn foo_args(args: *const libc::c_char) {
    println!("TESTE...");
    //println!("{:?}", args);
    let c_str = unsafe { CStr::from_ptr(args) };
    let r_str = c_str.to_string_lossy();
//    println!("{:?}", c_str.to_str());
    println!("{}", r_str);
}

