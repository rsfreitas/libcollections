
//! # Rust libcollections utility functions
//!
//! Utility functions to be used inside Rust plugins.

extern crate rustc_serialize;

use std::ffi::CStr;
use self::rustc_serialize::json::Json;

/// Converts a C string received as argument into a rustc-serialize Json
/// object.
pub fn retrieve_c_arguments_as_json(args: *const i8) -> Result<Json, i32> {
    let str_args = unsafe {
        if args.is_null() {
            return Err(-1);
        }

        CStr::from_ptr(args).to_string_lossy()
    };

    match Json::from_str(str_args.as_ref()) {
        Ok(value) => Ok(value),
        Err(_) => Err(-2),
    }
}

/// Converts a Rust string into a rustc-serialize Json object.
pub fn retrieve_argument_as_json(args: &str) -> Result<Json, i32> {
    match Json::from_str(args) {
        Ok(value) => Ok(value),
        Err(_) => Err(-1),
    }
}

