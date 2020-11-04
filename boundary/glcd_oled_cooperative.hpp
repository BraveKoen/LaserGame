#ifndef GLCD_OLED_COOPERATIVE_HPP
#define GLCD_OLED_COOPERATIVE_HPP

#include "hwlib.hpp"

class glcd_oled_cooperative :
    public hwlib::ssd1306_i2c,
    public hwlib::window
{
private:
    static auto constexpr wsize = hwlib::xy(128, 64);
    uint8_t buffer[wsize.x * wsize.y / 8];

    void write_implementation(
        hwlib::xy pos,
        hwlib::color col
    ) override {
        int a = pos.x + (pos.y / 8) * size.x;

        if (col == hwlib::white) {
            buffer[a] |= (0x01 << (pos.y % 8));
        } else {
            buffer[a] &= ~(0x01 << (pos.y % 8));
        }
    }
public:
    glcd_oled_cooperative(hwlib::i2c_bus& bus, int address = 0x3C):
        ssd1306_i2c(bus, address),
        window(wsize, hwlib::white, hwlib::black)
    {
        bus.write(address).write(
            hwlib::ssd1306_initialization,
            sizeof(hwlib::ssd1306_initialization)
            / sizeof(*hwlib::ssd1306_initialization));
    }

    void flush() override {
        command(hwlib::ssd1306_commands::column_addr, 0, size.x - 1);
        command(hwlib::ssd1306_commands::page_addr, 0, size.y / 8 - 1);

        if constexpr (true) {
            auto transaction = bus.write(address);
            transaction.write(hwlib::ssd1306_data_prefix);

            for (auto y = 0; y < size.y / 8; y++) {
                transaction.write(buffer + size.x * y, size.x);
                hwlib::wait_us(1);
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
