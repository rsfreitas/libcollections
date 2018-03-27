
//! # Rust libcollections API to retrieve arguments inside plugin functions
//!
//! API to retrieve arguments inside plugin functions.

extern crate libc;

use std::ffi::CStr;
use std::ffi::CString;
use std::os::raw::c_char;
use std::os::raw::c_void;

extern {
    // libcollection's function
    fn cl_get_last_error() -> i32;
    fn cl_plugin_argument_char(args: *const c_void, argument_name: *const c_char) -> i8;
    fn cl_plugin_argument_uchar(args: *const c_void, argument_name: *const c_char) -> u8;
    fn cl_plugin_argument_sint(args: *const c_void, argument_name: *const c_char) -> i16;
    fn cl_plugin_argument_usint(args: *const c_void, argument_name: *const c_char) -> u16;
    fn cl_plugin_argument_long(args: *const c_void, argument_name: *const c_char) -> i32;
    fn cl_plugin_argument_ulong(args: *const c_void, argument_name: *const c_char) -> u32;
    fn cl_plugin_argument_llong(args: *const c_void, argument_name: *const c_char) -> i64;
    fn cl_plugin_argument_ullong(args: *const c_void, argument_name: *const c_char) -> u64;
    fn cl_plugin_argument_int(args: *const c_void, argument_name: *const c_char) -> isize;
    fn cl_plugin_argument_uint(args: *const c_void, argument_name: *const c_char) -> usize;
    fn cl_plugin_argument_float(args: *const c_void, argument_name: *const c_char) -> f32;
    fn cl_plugin_argument_double(args: *const c_void, argument_name: *const c_char) -> f64;
    fn cl_plugin_argument_bool(args: *const c_void, argument_name: *const c_char) -> bool;
    fn cl_plugin_argument_pointer(args: *const c_void, argument_name: *const c_char, out: *mut *mut libc::c_void) -> i32;
    fn cl_plugin_argument_string(args: *const c_void, argument_name: *const c_char) -> *mut c_char;
}

/// Retrieves an argument from a plugin call as an i8 type.
pub fn retrieve_i8_argument(args: *const c_void, argument_name: &str) -> Result<i8, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_char(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an u8 type.
pub fn retrieve_u8_argument(args: *const c_void, argument_name: &str) -> Result<u8, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_uchar(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an i16 type.
pub fn retrieve_i16_argument(args: *const c_void, argument_name: &str) -> Result<i16, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_sint(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an u16 type.
pub fn retrieve_u16_argument(args: *const c_void, argument_name: &str) -> Result<u16, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_usint(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an i32 type.
pub fn retrieve_i32_argument(args: *const c_void, argument_name: &str) -> Result<i32, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_long(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an u32 type.
pub fn retrieve_u32_argument(args: *const c_void, argument_name: &str) -> Result<u32, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_ulong(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an i64 type.
pub fn retrieve_i64_argument(args: *const c_void, argument_name: &str) -> Result<i64, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_llong(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an u64 type.
pub fn retrieve_u64_argument(args: *const c_void, argument_name: &str) -> Result<u64, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_ullong(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an isize type.
pub fn retrieve_isize_argument(args: *const c_void, argument_name: &str) -> Result<isize, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_int(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an usize type.
pub fn retrieve_usize_argument(args: *const c_void, argument_name: &str) -> Result<usize, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_uint(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as a f32 type.
pub fn retrieve_f32_argument(args: *const c_void, argument_name: &str) -> Result<f32, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_float(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as a f64 type.
pub fn retrieve_f64_argument(args: *const c_void, argument_name: &str) -> Result<f64, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_double(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as a bool type.
pub fn retrieve_bool_argument(args: *const c_void, argument_name: &str) -> Result<bool, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let i = cl_plugin_argument_bool(args, aname.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as a pointer type.
pub fn retrieve_pointer_argument(args: *const c_void, argument_name: &str) -> Result<*const u8, i32> {
    let aname = CString::new(argument_name).unwrap();

    unsafe {
        let mut p = 0 as *mut u8;

        cl_plugin_argument_pointer(args, aname.as_ptr(),
                                   (&mut p) as *mut _ as *mut *mut libc::c_void);

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        return Ok(p)
    }
}

/// Retrieves an argument from a plugin call as a string.
pub fn retrieve_pointer_str(args: *const c_void, argument_name: &str) -> Result<&str, i32> {
    let aname = CString::new(argument_name).unwrap();
    let p = unsafe {
        let s = cl_plugin_argument_string(args, aname.as_ptr());
        CStr::from_ptr(s)
    };

    match p.to_str() {
        Ok(value) => Ok(value),
        Err(_) => Err(-1),
    }
}

