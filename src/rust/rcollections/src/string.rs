
//!
//! # Rust libcollections API to handle cl_string_t objects.
//!

use std::ffi::CString;
use std::ffi::CStr;
use std::os::raw::c_char;

extern {
    fn cl_string_valueof(s: *const u8) -> *const c_char;
    fn cl_string_set_content(s: *mut u8, content: *const u8) -> i32;
}

pub struct ClString {
    data: *mut u8,
}

impl ClString {
    pub fn get(&self) -> &str {
        let c = unsafe {
            let s = cl_string_valueof(self.data);
            CStr::from_ptr(s)
        };

        c.to_str().unwrap()
    }

    pub fn set(&self, content: &str) {
        let c = CString::new(content).unwrap();

        unsafe {
            cl_string_set_content(self.data, c.as_ptr() as *const u8);
        }
    }

    pub fn new(data: *mut u8) -> ClString {
        ClString{
            data: data,
        }
    }
}

