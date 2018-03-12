
//! # Rust libcollections API to retrieve arguments inside plugin functions
//!
//! API to retrieve arguments inside plugin functions.

// TODO: Correctly the error checking inside functions...

extern {
    // libcollection function's
    fn cl_get_last_error() -> i32;
    fn cl_plugin_argument_char(args: *const u8, argument_name: *const u8) -> i8;
    fn cl_plugin_argument_uchar(args: *const u8, argument_name: *const u8) -> u8;
    fn cl_plugin_argument_sint(args: *const u8, argument_name: *const u8) -> i16;
    fn cl_plugin_argument_usint(args: *const u8, argument_name: *const u8) -> u16;
    fn cl_plugin_argument_long(args: *const u8, argument_name: *const u8) -> i32;
    fn cl_plugin_argument_ulong(args: *const u8, argument_name: *const u8) -> u32;
    fn cl_plugin_argument_llong(args: *const u8, argument_name: *const u8) -> i64;
    fn cl_plugin_argument_ullong(args: *const u8, argument_name: *const u8) -> u64;
    fn cl_plugin_argument_int(args: *const u8, argument_name: *const u8) -> isize;
    fn cl_plugin_argument_uint(args: *const u8, argument_name: *const u8) -> usize;
    fn cl_plugin_argument_float(args: *const u8, argument_name: *const u8) -> f32;
    fn cl_plugin_argument_double(args: *const u8, argument_name: *const u8) -> f64;
    fn cl_plugin_argument_bool(args: *const u8, argument_name: *const u8) -> bool;
}

/// Retrieves an argument from a plugin call as an i8 type.
pub fn retrieve_i8_argument(args: *const u8, argument_name: &[u8]) -> Result<i8, i32> {
    unsafe {
        let i = cl_plugin_argument_char(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an u8 type.
pub fn retrieve_u8_argument(args: *const u8, argument_name: &[u8]) -> Result<u8, i32> {
    unsafe {
        let i = cl_plugin_argument_uchar(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an i16 type.
pub fn retrieve_i16_argument(args: *const u8, argument_name: &[u8]) -> Result<i16, i32> {
    unsafe {
        let i = cl_plugin_argument_sint(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an u16 type.
pub fn retrieve_u16_argument(args: *const u8, argument_name: &[u8]) -> Result<u16, i32> {
    unsafe {
        let i = cl_plugin_argument_usint(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an i32 type.
pub fn retrieve_i32_argument(args: *const u8, argument_name: &[u8]) -> Result<i32, i32> {
    unsafe {
        let i = cl_plugin_argument_long(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an u32 type.
pub fn retrieve_u32_argument(args: *const u8, argument_name: &[u8]) -> Result<u32, i32> {
    unsafe {
        let i = cl_plugin_argument_ulong(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an i64 type.
pub fn retrieve_i64_argument(args: *const u8, argument_name: &[u8]) -> Result<i64, i32> {
    unsafe {
        let i = cl_plugin_argument_llong(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an u64 type.
pub fn retrieve_u64_argument(args: *const u8, argument_name: &[u8]) -> Result<u64, i32> {
    unsafe {
        let i = cl_plugin_argument_ullong(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an isize type.
pub fn retrieve_isize_argument(args: *const u8, argument_name: &[u8]) -> Result<isize, i32> {
    unsafe {
        let i = cl_plugin_argument_int(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as an usize type.
pub fn retrieve_usize_argument(args: *const u8, argument_name: &[u8]) -> Result<usize, i32> {
    unsafe {
        let i = cl_plugin_argument_uint(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as a f32 type.
pub fn retrieve_f32_argument(args: *const u8, argument_name: &[u8]) -> Result<f32, i32> {
    unsafe {
        let i = cl_plugin_argument_float(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as a f64 type.
pub fn retrieve_f64_argument(args: *const u8, argument_name: &[u8]) -> Result<f64, i32> {
    unsafe {
        let i = cl_plugin_argument_double(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

/// Retrieves an argument from a plugin call as a bool type.
pub fn retrieve_bool_argument(args: *const u8, argument_name: &[u8]) -> Result<bool, i32> {
    unsafe {
        let i = cl_plugin_argument_bool(args, argument_name.as_ptr());

        if cl_get_last_error() != 0 {
            return Err(-1)
        }

        Ok(i)
    }
}

