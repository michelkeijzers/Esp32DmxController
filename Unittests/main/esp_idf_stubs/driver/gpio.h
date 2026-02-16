#pragma once
// Minimal stub for ESP-IDF driver/gpio.h
typedef struct
{
    unsigned long long pin_bit_mask;
    int mode;
    int pull_up_en;
    int pull_down_en;
    int intr_type;
} gpio_config_t;
typedef int gpio_num_t;
#define GPIO_NUM_1 1
#define GPIO_NUM_2 2
#define GPIO_NUM_3 3
#define GPIO_NUM_4 4
#define GPIO_NUM_5 5
#define GPIO_NUM_6 6
#define GPIO_NUM_7 7
#define GPIO_NUM_8 8
#define GPIO_NUM_9 9

#define GPIO_MODE_INPUT 0
#define GPIO_MODE_OUTPUT 1
#define GPIO_PULLUP_ENABLE 1
#define GPIO_PULLUP_DISABLE 0
#define GPIO_PULLDOWN_ENABLE 1
#define GPIO_PULLDOWN_DISABLE 0
#define GPIO_INTR_DISABLE 0
#define GPIO_INTR_ANYEDGE 3
#define GPIO_NUM_NC (-1)
#ifdef __cplusplus
extern "C"
{
#endif

    static inline int gpio_set_level(int gpio, int level) { return 0; }
    static inline int gpio_config(const gpio_config_t *) { return 0; }
    static inline int gpio_get_level(int gpio) { return 0; }
    static inline int gpio_install_isr_service(int gpio) { return 0; }
    static inline int gpio_isr_handler_add(int gpio, void (*)(void *), void *) { return 0; }

#ifdef __cplusplus
}
#endif
