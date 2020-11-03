#ifndef TRIGGER_HPP
#define TRIGGER_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../interface/button.hpp"

class Trigger: public Button, public rtos::task<> {
private:
    int buttonID;
    hwlib::target::pin_in sw;
    rtos::clock buttonClock; // update name in CCD accordingly
public:
    Trigger(int buttonID, hwlib::target::pins sw_pin):
        Button(),
        task("trigger task"),
        buttonID{buttonID},
        sw{sw_pin},
        buttonClock(this, 100'000, "button clock")
    {}

    void main() override {
        enum class State {
            Inactive
        };
        for (State state = State::Inactive;;) {
            switch (state) {
            case State::Inactive: 
                wait(buttonClock);
                updateOnPressed(); 
                break;
            default: break;
            }
        }
    }
private:
    void updateOnPressed() override {
        if (not sw.read()) {
            update(buttonID);
        }
    }
};

#endif // TRIGGER_HPP
