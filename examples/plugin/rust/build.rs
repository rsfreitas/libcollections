extern crate cc;

fn main() {
    cc::Build::new()
        .file("src/call.c")
        .compile("libdouble.a");
}

