#![no_std]

extern "C" {
    fn _putchar(ch:char);
}

fn println(s: &str) {
    for ch in s.chars() {
        unsafe {
            _putchar(ch);
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn rust_test() -> u32 {
    let rect1 = 30;
	let rect2 = 40;
    println("hello rust\n");
    return rect1 * rect2;
}
