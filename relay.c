#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "relay.h"
#include "ws2812.pio.h"

const uint LED_PIN = 25;
const uint PIN_LOOKUP[] = {
    JDQ1, JDQ2, JDQ3, JDQ4, JDQ5, JDQ6, JDQ7, JDQ8
};
unsigned char relay_states = 0;
bool blink_led_on = false;
char line_buffer[MAX_LINE_LENGTH + 1];
char word_buffer[MAX_LINE_LENGTH + 1];
char * word_list[MAX_WORDS];

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

bool blink_callback(struct repeating_timer *t) {
    if (blink_led_on) {
        gpio_put(LED_PIN, 1);
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

void relaySetOrClear(int pin_num, bool set_value) {
    int pin_value;
    if (set_value) {
        pin_value = 1;
    }
    else {
        pin_value = 0;
    }
    int physical_pin = PIN_LOOKUP[pin_num % 8];
    gpio_put(physical_pin, pin_value);
    if (pin_value == 1) {
        relay_states |= 1UL << pin_num;
    }
    else {
        relay_states &= ~(1UL << pin_num);
    }
    uint red = (relay_states >> 6) * 64;
    uint green = (relay_states & 0x3F >> 3) * 32;
    uint blue = (relay_states & 0x07) * 32;
    put_pixel(urgb_u32(red, green, blue));

}

int COMMAND_TABLE_LEN = 2;
char * COMMAND_TABLE[] = {
    "help",
    "relay"
};

char* HELP =
    "Commands:\n"
    "help\n"
    "relay set <relay num>\n"
    "relay clear\n";

void commandHelp() {
    printf("%s", HELP);
}

int RELAY_COMMAND_TABLE_LEN = 2;
char * RELAY_COMMAND_TABLE[] = {
    "set",
    "clear"
};

bool commandRelays(int word_count, char* word_list[]) {
    char* command = word_list[0];
    char **sub_commands =  &word_list[1];
    int sub_count = word_count - 1;
    if (sub_count < 1) {
        return false;
    }
    bool set_value = false;
    if (isEqual(command, COMMAND_TABLE[0])) {
        set_value = true;
    }
    int relay_num = sub_commands[0] - "0" - 1;
    relaySetOrClear(relay_num, set_value);
    return true;
}

void commandsAll(int word_count, char* word_list[]) {
    char* command = word_list[0];
    char **sub_commands = &word_list[1];
    int sub_count = word_count - 1;
    bool command_result = true;
    for (int command_num = 0; command_num < COMMAND_TABLE_LEN; command_num++) {
        printf("DEBUG %d, cmd: %s\n match: %s\n", command_num, command, COMMAND_TABLE[command_num]);
        if (isEqual(command,COMMAND_TABLE[command_num]) == true) {
            printf("EQUALS\n");
            switch (command_num)
            {
            case 0:
                commandHelp();
                break;
            
            case 1:
                command_result = commandRelays(sub_count, sub_commands);
                break;
            
            default:
                commandHelp();
                break;
            }
            break;
        }
    }
    if (command_result == false) {
        commandHelp();
    }
}

int lineIntoWords(int max_words, char* word_list[], int line_length, char* line)
{
    // Split a \0 terminated line with words separated by spaces
    // into multiple short strings by replacing spaces with \0
    int word_count = 0;
    bool in_word = false;
    for (int index = 0; index < line_length; index++) {
        if (in_word) {
            if ((line[index] == ' ') || (line[index] == '\n')) {
                line[index] = '\0';
                in_word = false;
            }
            else {
                continue;
            }
        }
        else {
            // Not in a word
            if ((line[index] == ' ') || (line[index] == '\n')) {
                line[index] = '\0';
            }
            else {
                // start of a word
                in_word = true;
                word_list[word_count] = &line[index];
                word_count++;
                if (word_count >= max_words) {
                    break;
                }
            }

        }

    }
    return word_count;
}

int readLine(char *buffer, int length)
{
    // https://lindevs.com/turn-on-off-the-onboard-led-on-raspberry-pi-pico-via-usb
    int index = 0;
    while (index < length) {
        int c = getchar_timeout_us(100);
        if (c == PICO_ERROR_TIMEOUT) {
            continue;
        }
        if (c <= 0) {
            printf("DEBUG CHAR: %x:\n", c);
            continue;
        }
        buffer[index++] = (char) c;
        printf("%c", c);
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
            printf("%s END\n", line_buffer);
            strcpy(word_buffer, line_buffer);
            int word_count = lineIntoWords(MAX_WORDS, word_list, data_len, word_buffer);
            printf("Word count: %d\n", word_count);
            printf("COMMAND: %s\n", word_list[0]);
            commandsAll(word_count, word_list);
        }
    }
        
    sleep_ms(1000);
}

