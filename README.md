# Pi_Pico_Relay
Control of Waveshare Pi Pico relays with serial commands over USB

## Controller for Waveshare 8 port Pi Pico relay product
The 8-port relay board is provided with example software written in
C and MicroPython for controlling the relays.
The standard "C" program takes binary commands over the USB serial
interface.

This program provides an alternative human-readable command line
interface

## Commands
help - simple (and incomplete) help
relay set <single base-1 relay number> - update single relay
relay clear <single base-1 relay number> - update single relay
relay value <hex with bits for each relay> mask <bit-mask of relay pins to change>
    - update any number of relays
relay state? - return a hex value indicating state of all relays
