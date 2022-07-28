use autocxx::prelude::*;

include_cpp! {
    #include "ccapi_cpp/ccapi_session.h"
    #include "ccapi_cpp/ccapi_message.h"

    // safety!(unsafe)
    // generate_ns!("ccapi")
    generate!("ccapi::Event")
    // generate!("ccapi::Message")
    // block_constructors!("ccapi::Message")
}

#[cxx::bridge]
mod message_ffi {
    enum RecapType {
        UNKNOWN,
        /// normal data tick; not a recap
        NONE,
        /// generated on request by subscriber
        SOLICITED,
    }

    unsafe extern "C++" {
        type Message;
    }
}

pub use ffi::ccapi::*;
use message_ffi::RecapType;
