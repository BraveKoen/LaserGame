#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

class Display: public rtos::task<> {
private:
    hwlib::target::pin_oc scl;
    hwlib::target::pin_oc sda;
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

    void displayMessage(char const *message) {
        // string literals have static storage duration so
        // writing the address of it to a pool seems fine
        messagePool.write({
            .tag = MessageType::Tag::String,
            .type = {.string = message}
        });
        messageFlag.set();
    }

    void displayMessage(char value) {
        // note: should be compiled with -std=c++2a
        messagePool.write({
            .tag = MessageType::Tag::Letter,
            .type = {.letter = value}
        });
        messageFlag.set();
    }

    void displayMessage(int value) {
        // note: should be compiled with -std=c++2a
        messagePool.write({
            .tag = MessageType::Tag::Number,
            .type = {.number = value}
        });
        messageFlag.set();
    }

    void displayMessage(char const *message, int value) {
        // note: should be compiled with -std=c++2a
        messagePool.write({
            .tag = MessageType::Tag::Pair,
            .type = {.pair = {.string = message, .number = value}}
        });
        messageFlag.set();
    }

    void clear() {
        clearFlag.set();
    }

    void main() override {
        for (state = State::Inactive;;) {
            switch (state) {
            case State::Inactive       : inactive()       ; break;
            case State::Clearing       : clearing()       ; break;
            case State::WritingMessage : writingMessage() ; break;
            default: break;
            }
        }
    }
private:
    enum class State {
        Inactive,
        Clearing
        WritingMessage
    };
    // this type of construction ultimately saves
    // some additional pools and an extra flush
    struct MessageType {
        enum class Tag {
            String,
            Letter,
            Number,
            Pair
        } tag;
        union Type {
            char const *string;
            char letter;
            int number;
            struct {
                char const *string;
                int number;
            } pair;
        } type;
    };
    State state;
    MessageType message;

    // using messageType = char const*;
    rtos::pool<messageType> messagePool;
    rtos::flag messageFlag;
    rtos::flag clearFlag;

    hwlib::i2c_bus_bit_banged_scl_sda i2c_bus;
    hwlib::glcd_oled display;
    hwlib::font_default_8x8 font;
    hwlib::terminal_from terminal;

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
        switch (messageType.tag) {
        case MessageType::Tag::String:
            terminal << message.type.string;
            break;
        case MessageType::Tag::Letter:
            terminal << message.type.letter;
            break;
        case MessageType::Tag::Number:
            terminal << message.type.number;
            break;
        case MessageType::Tag::Pair:
            terminal
                << message.type.pair.string
                << message.type.pair.number;
            break;
        default: break;
        }
        state = State::Inactive;
    }
};

#endif // DISPLAY_HPP
