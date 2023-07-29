#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "relay.h"

const uint LED_PIN = 25;

bool blink_led_on = false;
char line_buffer[MAX_LINE_LENGTH + 1];


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

bool isEqual(const char *cmd, const char *buffer)
{
    return strcmp(buffer, cmd) == 0;
}

int readLine(char *buffer, int length)
{
    // https://lindevs.com/turn-on-off-the-onboard-led-on-raspberry-pi-pico-via-usb
    int index = 0;
    while (index < length) {
        int c = getchar_timeout_us(100);
        if (c < 0) {
            continue;
        }
        buffer[index++] = (char) c;
        if (c == '\n') {
            break;
        }

    }
    buffer[index] = '\0';

    return index;
}

int main() {

    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    struct repeating_timer timer;
    add_repeating_timer_ms(500, blink_callback, NULL, &timer);

    while (1) {
        uint16_t data_len = readLine(line_buffer, MAX_LINE_LENGTH);
        if (data_len > 0) {
            printf("%s", line_buffer);
        }
    }
        
    sleep_ms(1000);
}

