#ifndef KEYPAD_HPP
#define KEYPAD_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../interface/button.hpp"

class Keypad: public Button, public rtos::task<> {
private:
    hwlib::matrix_of_switches matrix;
    hwlib::keypad<16> keypad;
public:
    Keypad(
        hwlib::port_oc& out_port,
        hwlib::port_in& in_port
    ):
        Button(),
        rtos::task("keypad task"),
        matrix{out_port, in_port},
        keypad{matrix, "123A456B789C*0#D"}
    {}

    void main() override {
        enum class State {
            WaitOnKeyPress
        };
        for (State state = State::WaitOnKeyPress;;) {
            switch (state) {
            case State::WaitOnKeyPress: updateOnPressed(); break;
            default: break;
            }
        }
    }
private:
    void updateOnPressed() override {
        update(keypad.getc());
    }
};

#endif // KEYPAD_HPP
