
//!
//! # Rust libcollections API to handle cl_cfg_file_t objects.
//!

use std::ffi::CString;
use object::ClObject;

extern {
    fn cl_cfg_get_value(c: *const u8, block: *const u8, entry: *const u8) -> *mut u8;
    fn cl_cfg_set_value_ex(c: *const u8, block: *const u8, entry: *const u8,
                           content: *const u8) -> i32;
}

pub struct ClCfgFile {
    data: *const u8,
}

impl ClCfgFile {
    pub fn get(&self, block: &str, entry: &str) -> String {
        let b = CString::new(block).unwrap();
        let e = CString::new(entry).unwrap();

        let o = unsafe {
            cl_cfg_get_value(self.data, b.as_ptr() as *const u8,
                             e.as_ptr() as *const u8)
        };

        let obj = ClObject::new(o);
        let ret = obj.get();

        /* It's ugly but it's needed */
        obj.unref();
        ret
    }

    pub fn set(&self, block: &str, entry: &str, content: &str) {
        let b = CString::new(block).unwrap();
        let e = CString::new(entry).unwrap();
        let c = CString::new(content).unwrap();

        unsafe {
            cl_cfg_set_value_ex(self.data, b.as_ptr() as *const u8,
                                e.as_ptr() as *const u8,
                                c.as_ptr() as *const u8);
        }
    }

    pub fn new(data: *const u8) -> ClCfgFile {
        ClCfgFile{
            data: data,
        }
    }
}

