#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <array>
#include "hwlib.hpp"
#include "button_listener.hpp"

class Button {
public:
    Button(): count{} {}

    // could accept a reference instead of a pointer?
    void addButtonListener(ButtonListener *listener) {
        if (count == max_listeners) {
            count = 0;
        }
        buttonListeners[count++] = listener;
    }
protected:
    void update(int buttonID) {
        hwlib::cout << buttonID << hwlib::endl;
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

// class Button {
// public:
//     Button(): count{} {}
//
//     // could accept a reference instead of a pointer?
//     void addButtonListener(ButtonListener *listener) {
//         if (count == max_listeners) {
//             count = 0;
//         }
//         buttonListeners[count++] = listener;
//     }
//
//     void updateButtonID() {
//         auto const buttonID = updateButtonIdImplementation();
//         update(buttonID);
//     }
// private:
//     static constexpr auto max_listeners = 8;
//     std::array<ButtonListener*, max_listeners> buttonListeners;
//     size_t count;
//
//     void update(int buttonID) {
//         for (size_t index = 0; index < count; index++) {
//             buttonListeners[index].buttonPressed(buttonID);
//         }
//     }
//
//     virtual int updateButtonIdImplementation() = 0;
// };

#endif // BUTTON_HPP
