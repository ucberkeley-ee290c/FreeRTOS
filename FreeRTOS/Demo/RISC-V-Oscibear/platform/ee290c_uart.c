#include "ee290c_uart.h"

struct sifive_uart {
    uint32_t tx_data;
    uint32_t rx_data;
    uint32_t tx_control;
    uint32_t rx_control;
    uint32_t interrupt_enable;
    uint32_t interrupt_pending;
    uint32_t baud_rate_divisor;
};

typedef volatile struct sifive_uart * volatile_sifive_uart_t;
_Static_assert(sizeof(struct sifive_uart) == 0x1C, "UART is unexpected size!");

void
ee290c_uart_init(uintptr_t uart_addr) {
    volatile_sifive_uart_t uart = (volatile_sifive_uart_t)(uart_addr);
    uart->tx_control = 0x1; /* txen */
    uart->rx_control = 0x1; /* rxen */
    // uart->baud_rate_divisor = 
}

void
ee290c_uart_write_sync(uintptr_t uart_addr, char c) {
    volatile_sifive_uart_t uart = (volatile_sifive_uart_t)(uart_addr);
    /*
    SiFive docs recommend using amoswap to test for the full flag and write to
    the UART in one fell swoop. You could just as easily do it using a read and
    a write.
    */
    uint32_t zero = 0;
    while (!__atomic_compare_exchange_n(
        /* target   */ &uart->tx_data, 
        /* expected */ &zero, 
        /* desired  */ (uint32_t)c,
        /* weak     */ 0x1,
        /* success  */ __ATOMIC_RELAXED, 
        /* fail     */ __ATOMIC_RELAXED
    ));
}