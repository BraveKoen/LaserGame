#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "boundary/display.hpp"

class Display: public rtos::task<> {
public:
    Display(
        hwlib::target::pins scl_pin,
        hwlib::target::pins sda_pin
    ):
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
    hwlib::12c_bus_bit_banged_scl_sda i2c_bus;
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
        terminal << message;
        // mag er ge-switcht worden naar een andere state
        // zonder dat een event daar verantwoordelijk voor is?
        //
        // in de RTOS reader staat namelijk:
        // (de Design Like a Robot reader hint hier ook naar)
        //
        // "A classic way to translate a state diagram to a task is to make
        // an enumerate for the states, and have a big switch statement in
        // the for(;;) loop that selects on this enumerate. In each such case,
        // you code:
        //   • the entry actions of the state,
        //   • wait for the event that leaves the state,  <-----
        //   • perform the actions that are associated with leaving the state
        //   • and with the state change,
        //   • and finally set the new state.
        // Don’t forget the final break stement!"
        //
        // misschien even navragen bij Marius?
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
        // literal strings have static storage duration so
        // writing the addresses of it to a pool seems fine
        messagePool.write(message);
        messageFlag.set();
    }

    void clear() {
        clearFlag.set();
    }
};

#endif // DISPLAY_HPP
