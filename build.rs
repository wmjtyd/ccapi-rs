use std::{env, path::Path};

fn main() {
    let cpp_root_path = Path::new("vendor/ccapi/");
    let cpp_header_path = cpp_root_path.join("include/");

    cxx_build::bridge("src/lib.rs")
        .include(&cpp_header_path)
        .include("cxx/")
        .compile("ccapi");

    println!("cargo:rerun-if-changed=src/");
    println!("cargo:rerun-if-changed=cxx/");
    println!("cargo:rerun-if-changed=vendor/ccapi/");
}
