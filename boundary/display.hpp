#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "glcd_oled_cooperative.hpp"

class Display: public rtos::task<> {
private:
    hwlib::target::pin_oc scl;
    hwlib::target::pin_oc sda;
public:
    enum class Font: uint8_t {
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
        font{
            hwlib::font_default_8x8(),
            hwlib::font_default_16x16()
        },
        terminal{
            .mode8x8{display, font.mode8x8},
            .mode16x16{display, font.mode16x16}
        },
        messagePool("message pool"),
        messageFlag(this, "message flag"),
        clearFlag(this, "clear flag")
    {
        //display.clear();
        //auto const *str = "wx: ";
        //Display::messageType = {str, Font::Mode8x8};
        //messagePool.write(messageType);

        //display.flush();
        //terminal.mode8x8 << messagePool.read().type.string;
        //terminal.mode8x8 << "\nnope\ndone8\n";
        //display.flush();
        //hwlib::wait_ms_busy(5'000);
    }

    void displayMessage(
        char const *message,
        Font font = Font::Mode16x16
    ) {
        messagePool.write({message, font});
        //static MessageType messageType{message, font};
        //messagePool.write(messageType);
        messageFlag.set();
    }

    void displayMessage(
        char value,
        Font font = Font::Mode16x16
    ) {
        messagePool.write({value, font});
        //static MessageType messageType{value, font};
        //messagePool.write(messageType);
        messageFlag.set();
    }

    void displayMessage(
        int value,
        Font font = Font::Mode16x16
    ) {
        messagePool.write({value, font});
        //static MessageType messageType{value, font};
        //messagePool.write(messageType);
        messageFlag.set();
    }

    void displayMessage(
        char const *message,
        int value,
        Font font = Font::Mode16x16
    ) {
        messagePool.write({message, value, font});
        //static MessageType messageType{message, value, font};
        //messagePool.write(messageType);
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
    enum class State: uint8_t {
        Inactive,
        Clearing
    };

    struct MessageType {
        MessageType(char const *string, Font font):
            tag{Tag::String},
            type{.string{string}},
            font{font}
        {}
        MessageType(char letter, Font font):
            tag{Tag::Letter},
            type{.letter{letter}},
            font{font}
        {}
        MessageType(int number, Font font):
            tag{Tag::Number},
            type{.number{number}},
            font{font}
        {}
        MessageType(char const *string, int number, Font font):
            tag{Tag::Pair},
            type{.pair{string, number}},
            font{font}
        {}
        MessageType() = default;

        enum class Tag: uint8_t {
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

    hwlib::i2c_bus_bit_banged_scl_sda i2cBus;
    std::conditional_t<
        true,
        glcd_oled_cooperative,
        hwlib::glcd_oled
    > display;
    struct {
        hwlib::font_default_8x8 mode8x8;
        hwlib::font_default_16x16 mode16x16;
    } font;
    struct {
        hwlib::terminal_from mode8x8;
        hwlib::terminal_from mode16x16;
    } terminal;
    //static MessageType messageType;
    State state;

    rtos::pool<MessageType> messagePool;
    rtos::flag messageFlag;
    rtos::flag clearFlag;

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
        //hwlib::cout << hwlib::endl; // change this <---------
        state = State::Inactive;
    }

    void writeMessage() {
        auto const& message = messagePool.read();
        auto& output{
            //hwlib::cout // change this  <-----------------------
            message.font == Font::Mode16x16
                ? terminal.mode16x16
                : terminal.mode8x8
        };
        switch (message.tag) {
        case MessageType::Tag::String:
            //hwlib::cout << "string" << message.type.string;
            //terminal.mode8x8 << "test";
            output << message.type.string;
            break;
        case MessageType::Tag::Letter:
            //hwlib::cout << "letter";
            output << message.type.letter;
            break;
        case MessageType::Tag::Number:
            //hwlib::cout << "number";
            output << message.type.number;
            break;
        case MessageType::Tag::Pair:
            //hwlib::cout << "pair";
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
