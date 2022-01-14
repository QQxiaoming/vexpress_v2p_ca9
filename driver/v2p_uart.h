#ifndef V2P_UART_H
#define V2P_UART_H

#include "core_ca.h"
#include "irq_ctrl.h"

int pl01x_getc(uint32_t addr, char *c, uint32_t timeout);
int pl01x_putc(uint32_t uart_addr, char c);
void pl01x_init(uint32_t uart_addr, int bd);

#endif /*V2P_UART_H*/
