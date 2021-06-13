use core::cell::UnsafeCell;
use core::ptr;
use core::marker::PhantomData;

#[repr(transparent)]
pub struct VolatileCell<T> {
    value: UnsafeCell<T>,
}

impl<T> VolatileCell<T> {
    /// Creates a new `VolatileCell` containing the given value
    pub const fn new(value: T) -> Self {
        VolatileCell { value: UnsafeCell::new(value) }
    }

    /// Returns a copy of the contained value
    #[inline(always)]
    pub fn get(&self) -> T
        where T: Copy
    {
        unsafe { ptr::read_volatile(self.value.get()) }
    }

    /// Sets the contained value
    #[inline(always)]
    pub fn set(&self, value: T)
        where T: Copy
    {
        unsafe { ptr::write_volatile(self.value.get(), value) }
    }

    /// Returns a raw pointer to the underlying data in the cell
    #[inline(always)]
    pub fn as_ptr(&self) -> *mut T {
        self.value.get()
    }
}

pub struct Reg<U> {
    register: VolatileCell<U>,
}

impl<U> Reg<U>
where
    U: Copy
{
    #[inline(always)]
    pub fn read(&self) -> U {
        self.register.get()
    }
}

#[repr(C)]
pub struct UartRegisterBlock {
    pub dr: Reg<u32>,        /* 0x00 Data register */    
    pub ecr: Reg<u32>,       /* 0x04 Error clear register (Write) */  
    pub pl010_lcrh: Reg<u32>,/* 0x08 Line control register, high byte */
    pub pl010_lcrm: Reg<u32>,/* 0x0C Line control register, middle byte */
    pub pl010_lcrl: Reg<u32>,/* 0x10 Line control register, low byte */
    pub pl010_cr: Reg<u32>,  /* 0x14 Control register */
    pub fr: Reg<u32>,        /* 0x18 Flag register (Read only) */   
    pub pl011_rlcr: Reg<u32>,/* 0x1c Receive line control register */
    pub ilpr: Reg<u32>,      /* 0x20 IrDA low-power counter register */ 
    pub pl011_ibrd: Reg<u32>,/* 0x24 Integer baud rate register */
    pub pl011_fbrd: Reg<u32>,/* 0x28 Fractional baud rate register */
    pub pl011_lcrh: Reg<u32>,/* 0x2C Line control register */
    pub pl011_cr: Reg<u32>,  /* 0x30 Control register */
}

pub struct USART0 {
    _marker: PhantomData<*const ()>,
}

impl USART0 {
    #[inline(always)]
    pub const fn ptr() -> *const UartRegisterBlock {
        0xf000_9000 as *const _
    }
}
