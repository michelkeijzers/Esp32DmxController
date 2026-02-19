#pragma once
// Minimal stub for ESP-IDF uart.h for unit testing
#ifdef __cplusplus
extern "C"
{
#endif

    typedef int uart_port_t;
#define UART_NUM_1 1
#define UART_PIN_NO_CHANGE (-1)
#define UART_DATA_8_BITS 8
#define UART_PARITY_DISABLE 0
#define UART_STOP_BITS_2 2
#define UART_HW_FLOWCTRL_DISABLE 0

    typedef struct
    {
        int baud_rate;
        int data_bits;
        int parity;
        int stop_bits;
        int flow_ctrl;
    } uart_config_t;

    static inline int uart_param_config(uart_port_t, const uart_config_t *) { return 0; }
    static inline int uart_set_pin(uart_port_t, int, int, int, int) { return 0; }
    static inline int uart_driver_install(uart_port_t, int, int, int, void *, int) { return 0; }
    static inline int uart_driver_delete(uart_port_t) { return 0; }
    static inline int uart_write_bytes(uart_port_t, const char *, int) { return 0; }
    static inline int uart_wait_tx_done(uart_port_t, int) { return 0; }

#ifdef __cplusplus
}
#endif
