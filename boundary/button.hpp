#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <array>
#include "hwlib.hpp"
#include "../interface/button_listener.hpp"

class Button {
private:
    // note: max_listeners needs to be a power of 2
    static constexpr auto max_listeners = 8;
    std::array<ButtonListener*, max_listeners> buttonListeners;
    int buttonID;
    uint_fast8_t count;
    hwlib::target::pin_in sw;
public:
    Button(hwlib::target::pins sw_pin, int buttonID):
        sw{sw_pin},
        buttonID{buttonID},
        count{}
    {}

    // could accept references instead of pointers?
    void addButtonListener(ButtonListener *listener) {
        buttonListeners[count++] = listener;

        if (count >= max_listeners) {
            count = 0;
        }
    }

    void update() {
        if (not sw.read()) {
            for (uint_fast8_t index = 0; index < count; index++) {
                buttonListeners[index].buttonPressed(buttonID);
            }
        }
    }
};

#endif // BUTTON_LISTENER_HPP
