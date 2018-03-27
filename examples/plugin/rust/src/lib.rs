
extern crate libc;
extern crate rcollections;

use rcollections::utils;
use rcollections::arguments;
use rcollections::c_void;

extern {
    fn rust_call(ptr: *const u8);
}

/**
 *
 * Plugin information...
 *
 */
#[no_mangle]
pub extern "C" fn plugin_name() -> *const u8 {
    "RUST-PLUGIN\0".as_ptr()
}

#[no_mangle]
pub extern "C" fn plugin_version() -> *const u8 {
    "0.1\0".as_ptr()
}

#[no_mangle]
pub extern "C" fn plugin_author() -> *const u8 {
    "Rodrigo Freitas\0".as_ptr()
}

#[no_mangle]
pub extern "C" fn plugin_description() -> *const u8 {
    "Rust plugin example\0".as_ptr()
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

    let _arg1 = jargs.as_object().unwrap().get("arg1").unwrap().as_u64().unwrap();
//    println!("{}", arg1);
}

#[no_mangle]
pub extern "C" fn another_outside_api(args: *const c_void) -> i32 {
    let arg1 = match arguments::retrieve_isize_argument(args, "arg1") {
        Ok(value) => value,
        Err(_) => return -1,
    };

    println!("{:?}", arg1);
    return 421
}

struct Test {
    data: *const u8,
}

impl Test {
    fn call(&self) {
        unsafe {
            rust_call(self.data);
        }
    }

    fn new(args: *const c_void) -> Result<Test, i32> {
        let ptr = match arguments::retrieve_pointer_argument(args, "ptr") {
            Ok(value) => value,
            Err(_) => return Err(-1),
        };

        Ok(Test{ data: ptr })
    }
}

#[no_mangle]
pub extern "C" fn foo_pointer(args: *const c_void) {
    let t = Test::new(args).unwrap();
    t.call()
}

