#ifndef GLCD_OLED_COOPERATIVE_HPP
#define GLCD_OLED_COOPERATIVE_HPP

#include "hwlib.hpp"

class glcd_oled_cooperative:
    public glcd_oled_i2c_128x64_buffered
{
public:
    glcd_oled_cooperative(
        i2c_bus& bus,
        int address = 0x3C
    ):
        glcd_oled_i2c_128x64_buffered(bus, address)
    {}

    void flush() override {
        command(hwlib::ssd1306_commands::column_addr, 0, size.x - 1);
        command(hwlib::ssd1306_commands::page_addr, 0, size.y / 8 - 1);

        if constexpr (true) {
            auto transaction = bus.write(address);
            transaction.write(hwlib::ssd1306_data_prefix);

            for (auto y = 0; y < size.y / 8; y++) {
                transaction.write(buffer + size.x * y, size.x);
                hwlib::wait_us(0);
            }
        } else {
            for (auto y = 0; y < size.y / 8; y++) {
                for (auto x = 0; x < size.x; x++) {
                    uint8_t data[]{
                        hwlib::ssd1306_data_prefix,
                        buffer[x + size.x * y]
                    };
                    bus.write(address).write(
                        data,
                        sizeof(data) / sizeof(*data));
                }
                hwlib::wait_us(0);
            }
        }
    }
 };

 #endif // GLCD_OLED_COOPERATIVE_HPP
