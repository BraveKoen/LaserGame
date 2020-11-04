#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "glcd_oled_cooperative.hpp"

/// \brief
/// Class Display
/// \details
/// .
class Display: public rtos::task<> {
private:
    hwlib::target::pin_oc scl;
    hwlib::target::pin_oc sda;
public:
    enum class Font: uint8_t {
        Mode8x8,
        Mode16x16
    };
/// \brief
/// Constructor Display
/// \details
/// This constructor has sclPin and a sdaPin
/// messagePool, messageFlag and clearFlag are created.
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
    {}

/// \brief
/// Function displayMessage needs a char message and a font
/// \details
/// This constructor has gameInfo, shotControl and Display by reference.
/// The constructor will also make buzzer with pin d9.
/// hitReceivedChannel, gameOverFlag and startFlag are created.
    void displayMessage(
        char const *message,
        Font font = Font::Mode16x16
    ) {
        messagePool.write({message, font});
        messageFlag.set();
    }

    void displayMessage(
        char letter,
        Font font = Font::Mode16x16
    ) {
        messagePool.write({letter, font});
        messageFlag.set();
    }

    void displayMessage(
        int number,
        Font font = Font::Mode16x16
    ) {
        messagePool.write({number, font});
        messageFlag.set();
    }

    void displayMessage(
        char const *message,
        int value,
        Font font = Font::Mode16x16
    ) {
        messagePool.write({message, value, font});
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
        state = State::Inactive;
    }

    void writeMessage() {
        auto const& message = messagePool.read();
        auto& output{
            message.font == Font::Mode16x16
                ? terminal.mode16x16
                : terminal.mode8x8
        };
        switch (message.tag) {
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
