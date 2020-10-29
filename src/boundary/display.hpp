#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

class Display: public rtos::task<> {
public:
    Display(
        hwlib::target::pins scl_pin,
        hwlib::target::pins sda_pin
    ):
        task("display task"),
        scl(scl_pin),
        sda(sda_pin),
        i2c_bus(scl, sda),
        display(i2c_bus),
        font(),
        terminal(display, font),
        messagePool("message pool"),
        messageFlag(this, "message flag"),
        clearFlag(this, "clear flag")
    {}
private:
    hwlib::target::pin_oc scl;
    hwlib::target::pin_oc sda;
    hwlib::i2c_bus_bit_banged_scl_sda i2c_bus;
    hwlib::glcd_oled display;
    hwlib::font_default_8x8 font;
    hwlib::terminal_from terminal;

    using messageType = char const*;
    rtos::pool<messageType> messagePool;
    rtos::flag messageFlag;
    rtos::flag clearFlag;

    enum class State {
        Inactive,
        Clearing
        WritingMessage
    };
    State state;
    messageType message;

    void inactive() {
        display.flush();
        auto const event = wait(clearFlag + messageFlag);

        if (event == clearFlag) {
            state = State::Clearing;
        } else if (event == messageFlag) {
            message = messagePool.read();
            state = State::WritingMessage;
        }
    }

    void clearing() {
        display.clear();
        state = State::Inactive;
    }

    void writingMessage() {
        // note: currently, there are no wait calls active at
        // glcd_i2c's flush implementation
        //
        // to-do: edit flush() function of glcd_oled_i2c_128x64_buffered class
        // file: hwlib-glcd-oled.hpp
        // line: 459 
        terminal << message;
        state = State::Inactive;
    }
public:
    void main() override {
        state = State::Inactive;

        for (;;) {
            switch (state) {
            case State::Inactive       : inactive()       ; break;
            case State::Clearing       : clearing()       ; break;
            case State::WritingMessage : writingMessage() ; break;
            default: break;
            }
        }
    }

    void displayMessage(messageType message) {
        // string literals have static storage duration so
        // writing the addresses of it to a pool seems fine
        messagePool.write(message);
        messageFlag.set();
    }

    void clear() {
        clearFlag.set();
    }
};

#endif // DISPLAY_HPP
