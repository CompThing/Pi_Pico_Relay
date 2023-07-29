#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

const uint LED_PIN = 25;

bool blink_led_on = false;

bool blink_callback(struct repeating_timer *t) {
    if (blink_led_on) {
        gpio_put(LED_PIN, 1);
        puts("Hello World\n");
        blink_led_on = false;
    }
    else {
        gpio_put(LED_PIN, 0);
        blink_led_on = true;
    }
    return true;
}

int main() {

    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    struct repeating_timer timer;
    add_repeating_timer_ms(500, blink_callback, NULL, &timer);

    int16_t ch = getchar_timeout_us(100);
    while (1) {
        ch = getchar_timeout_us(100);
        printf("%c", ch); 
        
    sleep_ms(1000);
    }
}

