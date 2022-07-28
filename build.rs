use std::path::Path;

use miette::IntoDiagnostic;

const INCLUDE_DIRS: [&str; 5] = [
    "cxx",
    "vendor/ccapi/include",
    "vendor/ccapi/dependency/websocketpp",
    "vendor/ccapi/dependency/boost",
    "vendor/ccapi/dependency/rapidjson/include",
    // Only need when we need FIX.
    // "vendor/ccapi/dependency/hffix/include",
];

fn main() -> miette::Result<()> {
    let mut path = INCLUDE_DIRS.into_iter().map(Path::new).collect::<Vec<_>>();

    // Pick the include path of OpenSSL.

    let openssl_inc = if let Ok(inc) = std::env::var("DEP_OPENSSL_INCLUDE") {
        inc
    } else {
        panic!("Unable to find the include directory of OpenSSL.");
    };

    path.push(Path::new(&openssl_inc));

    let mut b = autocxx_build::Builder::new("src/lib.rs", &path)
        .build()
        .into_diagnostic()?;

    b
        .flag_if_supported("-std=c++14")
        .flag_if_supported("-pthread")
        .flag_if_supported("-w")
        .compile("ccapi");

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=vendor/ccapi");

    // Generate the include path.
    let inc_path = path.into_iter()
        .map(|d| std::fs::canonicalize(d).map(|v| v.to_string_lossy().to_string()))
        .map(|p| p.into_diagnostic())
        .collect::<miette::Result<Vec<_>>>()?
        .join(",");

    println!("cargo:include={inc_path}");

    Ok(())
}
