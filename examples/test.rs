use ccapi_rs::{Event, Message};

fn main() {
    println!("{:?}", event);
}

fn event(e: Event) {
    let m = e.getMessageList().iter().map(|m| {
        // m.
    });
}
