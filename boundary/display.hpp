#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

class Display: public rtos::task<> {
private:
    hwlib::target::pin_oc scl;
    hwlib::target::pin_oc sda;
public:
    enum class Font : uint8_t {
        Mode8x8,
        Mode16x16
    };

    Display(
        hwlib::target::pins sclPin,
        hwlib::target::pins sdaPin
    ):
        task("display task"),
        scl(sclPin),
        sda(sdaPin),
        i2cBus(scl, sda),
        display(i2cBus),
        terminal{
            .mode8x8{display, hwlib::font_default_8x8()},
            .mode16x16{display, hwlib::font_default_16x16()}
        },
        messagePool("message pool"),
        messageFlag(this, "message flag"),
        clearFlag(this, "clear flag")
    {}

    void displayMessage(
        char const *message,
        Font font = Font::Mode16x16
    ) {
        // string literals have static storage duration so
        // writing the address of it to a pool seems fine
        // note: should be compiled with -std=c++2a
        messagePool.write({
            .tag = MessageType::Tag::String,
            .type = {.string = message},
            .font = font
        });
        messageFlag.set();
    }

    void displayMessage(
        char value,
        Font font = Font::Mode16x16
    ) {
        // note: should be compiled with -std=c++2a
        messagePool.write({
            .tag = MessageType::Tag::Letter,
            .type = {.letter = value},
            .font = font
        });
        messageFlag.set();
    }

    void displayMessage(
        int value,
        Font font = Font::Mode16x16
    ) {
        // note: should be compiled with -std=c++2a
        messagePool.write({
            .tag = MessageType::Tag::Number,
            .type = {.number = value},
            .font = font
        });
        messageFlag.set();
    }

    void displayMessage(
        char const *message,
        int value,
        Font font = Font::Mode16x16
    ) {
        // note: should be compiled with -std=c++2a
        messagePool.write({
            .tag = MessageType::Tag::Pair,
            .type = {.pair = {.string = message, .number = value}},
            .font = font
        });
        messageFlag.set();
    }

    void clear() {
        clearFlag.set();
    }

    void main() override {
        for (state = State::Inactive;;) {
            switch (state) {
            case State::Inactive: inactive(); break;
            case State::Clearing: clearing(); break;
            default: break;
            }
        }
    }
private:
    enum class State : uint8_t {
        Inactive,
        Clearing
    };
    // this type of construction ultimately saves
    // some additional pools and an extra flush
    struct MessageType {
        enum class Tag : uint8_t {
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
        Font font;
    };
    State state;

    rtos::pool<MessageType> messagePool;
    rtos::flag messageFlag;
    rtos::flag clearFlag;

    hwlib::i2c_bus_bit_banged_scl_sda i2cBus;
    hwlib::glcd_oled display;
    struct {
        hwlib::terminal_from mode8x8;
        hwlib::terminal_from mode16x16;
    } terminal;

    void inactive() {
        display.flush();
        auto const event = wait(clearFlag + messageFlag);

        if (event == clearFlag) {
            state = State::Clearing;
        } else if (event == messageFlag) {
            writeMessage();
        }
    }

    void clearing() {
        display.clear();
        state = State::Inactive;
    }

    void writeMessage() {
        // note: currently, there are no wait calls active at
        // glcd_i2c's flush implementation
        //
        // to-do: edit flush() function of glcd_oled_i2c_128x64_buffered class
        // file: hwlib-glcd-oled.hpp
        // line: 459
        auto const message = messagePool.read();
        auto const& output{
            message.font == Font::Mode16x16
                ? terminal.mode16x16
                : terminal.mode8x8
        };
        switch (messageType.tag) {
        case MessageType::Tag::String:
            output << message.type.string;
            break;
        case MessageType::Tag::Letter:
            output << message.type.letter;
            break;
        case MessageType::Tag::Number:
            output << message.type.number;
            break;
        case MessageType::Tag::Pair:
            output
                << message.type.pair.string
                << message.type.pair.number;
            break;
        default: break;
        }
        state = State::Inactive;
    }
};

#endif // DISPLAY_HPP
