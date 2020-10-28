#ifndef INIT_CONTROL_HPP
#define INIT_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "game_info.hpp"
#include "transfer_control.hpp"
#include "game_time_control.hpp"
#include "keypad.hpp"
#include "display.hpp"

class InitControl:
    public rtos::task<>,
    public ButtonListener
{
private:
    GameInfo& gameInfo;
    SendControl& sendControl;
    TransferControl& transferControl;
    GameTimeControl& gameTimeControl;
    Keypad& keypad;
    Display& display;

    using buttonType = int;
    rtos::channel<buttonType, 16> buttonChannel;
    rtos::clock countdownClock; // update name in CCD accordingly

    enum class MainState {
        ProcessInput,
        DistributeSettings,
        GameInProgress,
        CommandSelection
    };
    enum class SubState {
        RequestInput,
        AccumulateInput
    };
    MainState mainstate;
    SubState subState;

    char const *message;
    int minTime;
    int maxTime;
    int gameTime;
    int time;
    int countdown;
    bool confGameTime;
    bool countdownActive;
public:
    InitControl(
        char const* taskName,
        GameInfo& gameInfo,
        SendControl& sendControl,
        TransferControl& transferControl,
        GameTimeControl& gameTimeControl,
        Keypad& keypad,
        Display& display,
        ButtonHandler& handler
    ):
        task(taskName),
        gameInfo{gameInfo},
        sendControl{sendControl},
        transferControl{transferControl},
        gameTimeControl{gameTimeControl},
        keypad{keypad},
        display{display},
        buttonChannel(this, "button channel"),
        countdownClock(this, 1'000, "countdown clock")
    {
        keypad.addButtonListener(this);
        handler.addButton(keypad);
    }

    void buttonPressed(int buttonID) {
        buttonChannel.write(buttonID);
    }
private:
    void processInput(
        char const *message,
        int minTime,
        int maxTime
    ) {
        buttonType const buttonID;

        switch (subState) {
        case SubState::RequestInput:
            display.clear();
            display.displayMessage(message);
            buttonID = buttonChannel.read();
            time = 0;
            break;

        case SubState::AccumulateInput:
            buttonID = buttonChannel.read();
            break;

        default:
            break;
        }
        validateInput(buttonID);
    }

    void validateInput(char input) {
        if (input >= '0' and input <= '9') {
            // create overload for .display(char) ?
            // will need to change this somehow
            display.displayMessage(input);
            time = time * 10 + input - '0';
            subState = SubState::AccumulateInput;
        } else if (input == '#') {
            if (time >= minTime and time <= maxTime) {
                if (confGameTime) {
                    gameInfo.setTime(time);
                    gameTime = time;
                    message = "Enter countdown (5-30): ";
                    minTime = 5;
                    maxTime = 30;
                    confGameTime = false;
                    // mainState = MainState::ProcessInput;
                } else {
                    countdown = time;
                    countdownActive = false;
                    display.clear();
                    display.displayMessage(
                        "# - Speeltijd versturen\n"
                        "* - Countdown versturen"
                    );
                    mainState = MainState::DistributeSettings;
                }
            } else {
                display.clear();
                display.displayMessage("Invoer ongeldig!");
                hwlib::wait_ms(1'000);
                subState = SubState::RequestInput;
            }
        } else {
            subState = SubState::AccumulateInput;
        }
    }

    void distributeSettings() {
        auto const event = wait(buttonChannel, countdownClock);

        if (event == buttonChannel) {
            auto const buttonID = buttonChannel.read();
            char const *message;

            if (buttonID == '*') {
                sendControl.sendMessage(countdown << 6);
                message = "Countdown verstuurd!";
                countdownActive = true;
            } else if (buttonID == '#') {
                sendControl.sendMessage(gameTime << 6);
                sendControl.sendMessage(0b0);
                message = "Speeltijd verstuurd!";
            } else {
                // mainState = MainState::DistributeSettings;
                return;
            }
            display.clear();
            display.displayMessage(message);
            hwlib::wait_ms(1'000);
            display.clear();
            display.displayMessage(
                "# - Speeltijd versturen\n"
                "* - Countdown versturen");
        } /*else if (event == countdownClock) {
            if (countdownActive) {
                --countdown;

                if (countdown > 0) {
                    // mainState = MainState::DistributeSettings;
                } else {
                    mainState = MainState::GameInProgress;
                }
            } else {
                // mainState = MainState::DistributeSettings;
            }
        } */
        else if (event == countdownClock
            and countdownActive
            and --countdown < 1
        ) {
            mainState = MainState::GameTimeControl;
        }
    }

    void gameInProgress() {
        // add default parameter to GameTimeControl::start(countdown = 0) ?
        gameTimeControl.start(0);
        hwlib::wait_ms(gameTime * 60'000);
        display.clear();
        display.displayMessage(
            "C - Instellingen invoeren"
            "D - Spelgegevens transferen");
        mainState = MainState::CommandSelection;
    }

    void commandSelection() {
        auto const buttonID = buttonChannel.read();

        if (buttonID == 'D') {
            transferControl.transferCommand();
        } else if (buttonID == 'C') {
            message = "Enter speeltijd (1-15): ";
            minTime = 1;
            maxTime = 15;
            confGameTime = true;
            mainState = MainState::ProcessInput;
            subState = SubState::RequestInput;
        } /*else {
            mainState = MainState::CommandSelection;
        }*/
    }
public:
    void main() override {
        mainState = MainState::ProcessInput;
        subState = SubState::RequestInput;
        message = "Enter speeltijd (1-15): ";
        minTime = 1;
        maxTime = 15;
        confGameTime = true;

        for (;;) {
            switch (mainState) {
            case MainState::ProcessInput       : processInput()       ; break;
            case MainState::DistributeSettings : distributeSettings() ; break;
            case MainState::GameInProgress     : gameInProgress()     ; break;
            case MainState::CommandSelection   : commandSelection()   ; break;
            default: break;
            }
        }
    }
};

#endif // INIT_CONTROL_HPP
