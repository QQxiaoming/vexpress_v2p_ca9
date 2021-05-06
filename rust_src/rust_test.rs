#![no_std]

#[macro_use]
pub mod debug_log_rs;

use debug_log_rs::_print;
use core::mem;

#[no_mangle]
pub unsafe extern "C" fn rust_add(rect1: u32,rect2: u32) -> u32 {
    println!("rust_add rect1 is {} rect2 is {}", rect1, rect2);

    let ddr_p = 0x80008000 as *mut u32;
    *ddr_p.offset(0) = rect2;

    let mut x = 5;
    let mut y = 42;
    mem::swap(&mut x, &mut y);
    println!("x is {} y is {}", x, y);

    return rect1 * rect2;
}
