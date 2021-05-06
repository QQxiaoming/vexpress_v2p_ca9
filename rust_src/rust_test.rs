#![no_std]

#[macro_use]
pub mod debug_log_rs;

use debug_log_rs::_print;

#[no_mangle]
pub unsafe extern "C" fn rust_test() -> u32 {
    let rect1 = 30;
	let rect2 = 40;
    println!("hello rust rect1 is {} rect2 is {}", rect1, rect2);
    return rect1 * rect2;
}
