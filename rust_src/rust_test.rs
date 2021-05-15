#![no_std]

#[macro_use]
pub mod debug_log_rs;

use debug_log_rs::_print;
use core::mem;

use core::fmt::Display;

fn longest_with_an_announcement<'a, T>(x: &'a str, y: &'a str, ann: T) -> &'a str
    where T: Display
{
    println!("Announcement! {}", ann);
    if x.len() > y.len() {
        x
    } else {
        y
    }
}

trait Comparable {
    fn compare(&self, object: &Self) -> i8;
}

fn max<T: Comparable>(array: &[T]) -> &T {
    let mut max_index = 0;
    let mut i = 1;
    while i < array.len() {
        if array[i].compare(&array[max_index]) > 0 {
            max_index = i;
        }
        i += 1;
    }
    &array[max_index]
}

impl Comparable for f64 {
    fn compare(&self, object: &f64) -> i8 {
        if &self > &object { 1 }
        else if &self == &object { 0 }
        else { -1 }
    }
}

#[no_mangle]
pub unsafe extern "C" fn rust_add(rect1: u32,rect2: u32) -> u32 {
    println!("rust_add rect1 is {} rect2 is {}", rect1, rect2);

    let ddr_p = 0x80008000 as *mut u32;
    *ddr_p.offset(0) = rect2;

    let mut x = 5;
    let mut y = 42;
    mem::swap(&mut x, &mut y);
    println!("x is {} y is {}", x, y);

    let arr = [1.1, 3.3, 5.5, 4.4, 2.2];
    println!("maximum of arr is {}", max(&arr));

    let s0 = "s0";
    let s1 = "s1s1";
    let ann = 0.584;
    let ret = longest_with_an_announcement(s0,s1,ann);
    println!("ret is {}", ret);

    return rect1 * rect2;
}

pub mod rust_uart;
use rust_uart::USART0;

#[no_mangle]
pub unsafe extern "C" fn test_rust_uart() {
    println!("pl011_rlcr is {}", (*USART0::ptr()).pl011_rlcr.read());
    println!("pl011_ibrd is {}", (*USART0::ptr()).pl011_ibrd.read());
    println!("pl011_fbrd is {}", (*USART0::ptr()).pl011_fbrd.read());
    println!("pl011_lcrh is {}", (*USART0::ptr()).pl011_lcrh.read());
    println!("pl011_cr is {}", (*USART0::ptr()).pl011_cr.read());
}
