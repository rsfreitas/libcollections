
//!
//! # Rust libcollections API to handle cl_object_t objects.
//!

use std::ffi::CString;
use string::ClString;

extern {
    fn cl_object_to_cstring(o: *const u8) -> *mut u8;
    fn cl_object_set_ex(o: *const u8, content: *const u8) -> i32;
    fn cl_object_unref(o: *mut u8) -> i32;
}

pub struct ClObject {
    data: *mut u8,
}

impl ClObject {
    pub fn get(&self) -> String {
        let s = unsafe { cl_object_to_cstring(self.data) };
        let cl_string = ClString::new(s);
        cl_string.get().to_string()
    }

    pub fn set(&self, content: &str) {
        let c = CString::new(content).unwrap();

        unsafe {
            cl_object_set_ex(self.data, c.as_ptr() as *const u8);
        }
    }

    pub fn unref(&self) {
        unsafe {
            cl_object_unref(self.data);
        }
    }

    pub fn new(data: *mut u8) -> ClObject {
        ClObject{
            data: data,
        }
    }
}

