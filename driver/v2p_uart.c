#include "v2p_ca9_system.h"
#include "v2p_uart.h"

typedef struct
{
	__IOM uint32_t dr;		    /* 0x00 Data register */
	__IOM uint32_t ecr;		    /* 0x04 Error clear register (Write) */
	__IOM uint32_t pl010_lcrh;	/* 0x08 Line control register, high byte */
	__IOM uint32_t pl010_lcrm;	/* 0x0C Line control register, middle byte */
	__IOM uint32_t pl010_lcrl;	/* 0x10 Line control register, low byte */
	__IOM uint32_t pl010_cr;	/* 0x14 Control register */
	__IOM uint32_t fr;		    /* 0x18 Flag register (Read only) */
	__IOM uint32_t pl011_rlcr;	/* 0x1c Receive line control register */
	__IOM uint32_t ilpr;		/* 0x20 IrDA low-power counter register */
	__IOM uint32_t pl011_ibrd;	/* 0x24 Integer baud rate register */
	__IOM uint32_t pl011_fbrd;	/* 0x28 Fractional baud rate register */
	__IOM uint32_t pl011_lcrh;	/* 0x2C Line control register */
	__IOM uint32_t pl011_cr;	/* 0x30 Control register */
}pl01x_regs;

#define UART_PL01x_RSR_OE               0x08
#define UART_PL01x_RSR_BE               0x04
#define UART_PL01x_RSR_PE               0x02
#define UART_PL01x_RSR_FE               0x01

#define UART_PL01x_FR_TXFE              0x80
#define UART_PL01x_FR_RXFF              0x40
#define UART_PL01x_FR_TXFF              0x20
#define UART_PL01x_FR_RXFE              0x10
#define UART_PL01x_FR_BUSY              0x08
#define UART_PL01x_FR_TMSK              (UART_PL01x_FR_TXFF + UART_PL01x_FR_BUSY)

#define UART_PL011_LCRH_SPS             (1 << 7)
#define UART_PL011_LCRH_WLEN_8          (3 << 5)
#define UART_PL011_LCRH_WLEN_7          (2 << 5)
#define UART_PL011_LCRH_WLEN_6          (1 << 5)
#define UART_PL011_LCRH_WLEN_5          (0 << 5)
#define UART_PL011_LCRH_FEN             (1 << 4)
#define UART_PL011_LCRH_STP2            (1 << 3)
#define UART_PL011_LCRH_EPS             (1 << 2)
#define UART_PL011_LCRH_PEN             (1 << 1)
#define UART_PL011_LCRH_BRK             (1 << 0)

#define UART_PL011_CR_CTSEN             (1 << 15)
#define UART_PL011_CR_RTSEN             (1 << 14)
#define UART_PL011_CR_OUT2              (1 << 13)
#define UART_PL011_CR_OUT1              (1 << 12)
#define UART_PL011_CR_RTS               (1 << 11)
#define UART_PL011_CR_DTR               (1 << 10)
#define UART_PL011_CR_RXE               (1 << 9)
#define UART_PL011_CR_TXE               (1 << 8)
#define UART_PL011_CR_LPE               (1 << 7)
#define UART_PL011_CR_IIRLP             (1 << 2)
#define UART_PL011_CR_SIREN             (1 << 1)
#define UART_PL011_CR_UARTEN            (1 << 0)

#define UART_PL011_IMSC_OEIM            (1 << 10)
#define UART_PL011_IMSC_BEIM            (1 << 9)
#define UART_PL011_IMSC_PEIM            (1 << 8)
#define UART_PL011_IMSC_FEIM            (1 << 7)
#define UART_PL011_IMSC_RTIM            (1 << 6)
#define UART_PL011_IMSC_TXIM            (1 << 5)
#define UART_PL011_IMSC_RXIM            (1 << 4)
#define UART_PL011_IMSC_DSRMIM          (1 << 3)
#define UART_PL011_IMSC_DCDMIM          (1 << 2)
#define UART_PL011_IMSC_CTSMIM          (1 << 1)
#define UART_PL011_IMSC_RIMIM           (1 << 0)

int pl01x_getc(uint32_t uart_addr, char *c, uint32_t timeout)
{
    pl01x_regs *uart = (pl01x_regs *)uart_addr;
	
	while ((uart->fr & UART_PL01x_FR_RXFE) != 0){
		if(timeout != 0xFFFFFFFF) {
			if(timeout == 0) {
				return -1;
			}
			timeout--;
		}
	}
	if(c) {
		*c = uart->dr;
	}
	return 0;
}

int pl01x_putc(uint32_t uart_addr, char c)
{
    pl01x_regs *uart = (pl01x_regs *)uart_addr;
    if ((uart->fr) & UART_PL01x_FR_TXFF) return -1;

    uart->dr = c;

    return 0;
}

void pl01x_init(uint32_t uart_addr, int bd)
{
    pl01x_regs *uart = (pl01x_regs *)uart_addr;
    unsigned int temp;
    unsigned int divider;
    unsigned int remainder;
    unsigned int fraction;

	temp = 16 * bd;
    divider = 24000000 / temp;
    remainder = 24000000 % temp;
    temp = (8 * remainder) / bd;
    fraction = (temp >> 1) + (temp & 1);

    uart->pl011_cr = 0;
    uart->pl011_ibrd = divider;
    uart->pl011_fbrd = fraction;
    uart->pl011_lcrh = UART_PL011_LCRH_WLEN_8 | UART_PL011_LCRH_FEN;
    uart->pl011_cr = UART_PL011_CR_UARTEN | UART_PL011_CR_TXE |
		              UART_PL011_CR_RXE | UART_PL011_CR_RTS;
}
