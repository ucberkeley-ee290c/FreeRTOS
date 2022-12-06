#ifndef EE290C_UART_H
#define EE290C_UART_H
#include <stdint.h>

void
ee290c_uart_init(uintptr_t uart_addr);

void
ee290c_uart_write_sync(uintptr_t uart_addr, char c);

char
ee290c_uart_read_sync(uintptr_t uart_addr);

#endif /* EE290C_UART_H */