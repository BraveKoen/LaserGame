#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <array>
#include "hwlib.hpp"
#include "button_listener.hpp"

class Button {
public:
    Button(): count{} {}

    void addButtonListener(ButtonListener *listener) {
        if (count == max_listeners) {
            count = 0;
        }
        buttonListeners[count++] = listener;
    }
protected:
    void update(int buttonID) {
        for (size_t index = 0; index < count; index++) {
            buttonListeners[index]->buttonPressed(buttonID);
        }
    }
private:
    static constexpr auto max_listeners = 8;
    std::array<ButtonListener*, max_listeners> buttonListeners;
    size_t count;

    virtual void updateOnPressed() = 0;
};

#endif // BUTTON_HPP
