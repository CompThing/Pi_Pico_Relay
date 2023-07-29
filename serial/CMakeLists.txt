add_executable(relay_serial
        relay_serial.c
        )

# pull in common dependencies
target_link_libraries(relay_serial pico_stdlib)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(relay_serial)

# add url via pico_set_program_url
example_auto_set_url(relay_serial)
