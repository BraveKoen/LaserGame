#ifndef INIT_CONTROL_HPP
#define INIT_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../entity/game_info.hpp"
#include "game_time_control.hpp"
#include "transfer_control.hpp"
#include "send_control.hpp"

#include "../boundary/keypad.hpp"
#include "../boundary/display.hpp"
/// \brief
/// Class InitControl.
/// \details
/// This class is for game setting: Countdown and how long the game lasts
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
    // rtos::flag gameOverFlag;
    rtos::clock countdownClock; // update name in CCD accordingly
public:
/// \brief
/// Constructor ReceiveControl.
/// \details
/// the Constructor expects GameInfo, SendControl, TransferControl, GameTimeControl, Keypad and Display all by reference.
/// ButtonChannel and CountDownClock are created, the keypad is add to ButtonListener. 
    InitControl(
        GameInfo& gameInfo,
        SendControl& sendControl,
        TransferControl& transferControl,
        GameTimeControl& gameTimeControl,
        Keypad& keypad,
        Display& display
    ):
        task("init_control task"),
        gameInfo{gameInfo},
        sendControl{sendControl},
        transferControl{transferControl},
        gameTimeControl{gameTimeControl},
        keypad{keypad},
        display{display},
        buttonChannel(this, "button channel"),
        countdownClock(this, 1'000'000, "countdown clock")
    {
        keypad.addButtonListener(this);
    }
/// \brief
/// buttonPressed function void, expects a int
/// \details
/// buttonPressed needs a buttunID. When the function is called it will write the buttonID to the buttonChannel.
    void buttonPressed(int buttonID) override {
        buttonChannel.write(buttonID);
    }

    // void gameOver() {
    //     gameOverFlag.set();
    // }
private:
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
    MainState mainState;
    SubState subState;

    char const *message;
    uint_fast8_t minTime;
    uint_fast8_t maxTime;
    uint_fast8_t gameTime;
    uint_fast8_t time;
    uint_fast8_t countdown;
    uint_fast8_t inputSize;
    bool confGameTime;
    bool countdownActive;
    buttonType buttonID;

    void processInput() {
        switch (subState) {
        case SubState::RequestInput:
            //display.clear();
            display.displayMessage(message, Display::Font::Mode8x8);
            buttonID = buttonChannel.read();
            time = 0;
            inputSize = 0;
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
        if (input == '#' or inputSize > 4) {
            if (time >= minTime and time <= maxTime) {
                if (confGameTime) {
                    gameInfo.setTime(time);
                    gameTime = time;
                    initCountdownInput();
                    // mainState = MainState::ProcessInput;
                } else {
                    countdown = time;
                    countdownActive = false;
                    initDistributeSettings();
                    mainState = MainState::DistributeSettings;
                }
                // subState = SubState::RequestInput;
            } else {
                // display.clear();
                display.displayMessage("\f\vInvoer\nongeldig!", Display::Font::Mode8x8);
                hwlib::wait_ms(1'000);
                // subState = SubState::RequestInput;
            }
            subState = SubState::RequestInput;
        } else if (input >= '0' and input <= '9') {
            display.displayMessage(static_cast<char>(buttonID), Display::Font::Mode8x8);
            time = time * 10 + input - '0';
            inputSize++;
            subState = SubState::AccumulateInput;
        } else {
            subState = SubState::AccumulateInput;
        }
    }

    void distributeSettings() {
        auto event = wait(buttonChannel + countdownClock);

        if (event == buttonChannel) {
            auto const buttonID = buttonChannel.read();

            if (buttonID == '*') {
                sendControl.sendMessage(gameTime << 5);
                sendControl.sendMessage(0b0);
                sendControl.sendMessage(countdown << 5);
                countdownActive = true;
                // display.clear();
                display.displayMessage("\f\vSettings\nverstuurd!", Display::Font::Mode8x8);
                hwlib::wait_ms(1'000);
                initDistributeSettings();
            }
            // mainState = MainState::DistributeSettings;
        } else if (event == countdownClock
            and countdownActive
            and --countdown == 0
        ) {
            mainState = MainState::GameInProgress;
        }
    }

    void gameInProgress() {
        // add default parameter to GameTimeControl::start(countdown = 0) ?
        gameTimeControl.start(0);
        // wait(gameOverFlag);
        hwlib::wait_ms(gameTime * 60'000);
        initCommandSelection();
        mainState = MainState::CommandSelection;
    }

    void commandSelection() {
        auto const buttonID = buttonChannel.read();

        if (buttonID == 'C') {
            initGameTimeInput();
            mainState = MainState::ProcessInput;
            // subState = SubState::RequestInput;
        } else if (buttonID == 'D') {
            // while this action takes place, we probably want
            // to wait for a flag until TransferControl is done?
            transferControl.transferCommand();
            // mainState = MainState::CommandSelection;
        } else if (buttonID == '*') {
            sendControl.sendMessage(0b1000'10000);
            // display.clear();
            display.displayMessage("\f\vTransfer\nverstuurd!", Display::Font::Mode8x8);
            hwlib::wait_ms(1'000);
            initCommandSelection();
        }
    }

    void initGameTimeInput() {
        message = "\f\vEnter speel-\ntijd (1-15):\n";
        minTime = 1;
        maxTime = 15;
        confGameTime = true;
    }

    void initCountdownInput() {
        message = "\f\vEnter count-\ndown (5-30):\n";
        minTime = 5;
        maxTime = 30;
        confGameTime = false;
    }

    void initDistributeSettings() {
        // display.clear();
        display.displayMessage(
            "\f\v* - Settings\n    versturen",
            Display::Font::Mode8x8);
    }

    void initCommandSelection() {
        // display.clear();
        display.displayMessage(
            "\f\vC - Settings\n    invoeren\n"
            "D - Lokaal\n    transferen\n"
            "* - Transfer\n    versturen",
            Display::Font::Mode8x8);
    }
public:
    void main() override {
        mainState = MainState::ProcessInput;
        subState = SubState::RequestInput;
        initGameTimeInput();

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
