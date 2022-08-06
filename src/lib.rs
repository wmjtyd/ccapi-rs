#[cxx::bridge]
pub mod ffi {
    unsafe extern "C++" {
        include!("ccapi-rs/cxx/ITradeclient.h");

        type ITradeClient;
    }
}
