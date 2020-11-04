#ifndef INIT_CONTROL_HPP
#define INIT_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../entity/game_info.hpp"
#include "game_time_control.hpp"
#include "register_control.hpp"
#include "transfer_control.hpp"
#include "send_control.hpp"
#include "../boundary/keypad.hpp"
#include "../boundary/display.hpp"

class GameTimeControl;
class RegisterControl;

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
    Keypad& keypad;
    Display& display;
    GameTimeControl *gameTimeControl;
    RegisterControl *registerControl;

    using buttonType = int;
    rtos::channel<buttonType, 16> buttonChannel;
    rtos::clock countdownClock; // update name in CCD accordingly
    rtos::flag startMenuFlag;
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
        Keypad& keypad,
        Display& display
    );

    // scuffed workaround for missing a main controller GameControl
    void setGameTimeControl(GameTimeControl *timeControl);

    // scuffed workaround for missing a main controller GameControl
    void setRegisterControl(RegisterControl *regControl);

    void startMenu();

    void startRegisterMenu();

    void startGameTime(unsigned countdown);

    /// \brief
    /// buttonPressed function void, expects a int
    /// \details
    /// buttonPressed needs a buttunID. When the function is called it will write the buttonID to the buttonChannel.
    void buttonPressed(int buttonID) override;

    void main() override;
private:
    enum class MainState {
        Inactive,
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
    bool firstCountdown;

    void inactive() {
        wait(startMenuFlag);
        buttonChannel.clear();
        mainState = MainState::ProcessInput;
        subState = SubState::RequestInput;
        initGameTimeInput();
    }

    void processInput() {
        switch (subState) {
        case SubState::RequestInput:
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
        if (input == '#' or inputSize >= 3) {
            if (time >= minTime and time <= maxTime) {
                if (confGameTime) {
                    gameInfo.setTime(time);
                    gameTime = time;
                    initCountdownInput();
                    // mainState = MainState::ProcessInput;
                } else {
                    countdown = time;
                    countdownActive = false;
                    display.clear();
                    initDistributeSettings();
                    firstCountdown=true;
                    mainState = MainState::DistributeSettings;
                }
                // subState = SubState::RequestInput;
            } else {
                display.displayMessage(
                    "\t0003Input invalid!",
                    Display::Font::Mode8x8);
                hwlib::wait_ms(1'000);
                buttonChannel.clear();
                // subState = SubState::RequestInput;
            }
            subState = SubState::RequestInput;
        } else if (input >= '0' and input <= '9') {
            display.displayMessage(
                static_cast<char>(buttonID),
                Display::Font::Mode8x8
            );
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
                sendControl.sendMessage(0b0);
                sendControl.sendMessage(gameTime);
                sendControl.sendMessage(countdown);
                countdownActive = true;
                display.displayMessage(
                    "\t0005Settings sent!  "
                    "\n                ",
                    Display::Font::Mode8x8
                );
                if(firstCountdown){
                    startGameTime(countdown);
                    firstCountdown=false;
                }
                hwlib::wait_ms(1'000);
                buttonChannel.clear();
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
        wait(startMenuFlag);
        buttonChannel.clear();
        initCommandSelection();
        mainState = MainState::CommandSelection;
    }

    void commandSelection() {
        auto const buttonID = buttonChannel.read();

        if (buttonID == 'A') {
            startRegisterMenu();
            mainState = MainState::Inactive;
        } else if (buttonID == 'C') {
            initGameTimeInput();
            mainState = MainState::ProcessInput;
            // subState = SubState::RequestInput;
        } else if (buttonID == 'D') {
            // while this action takes place, we probably want
            // to wait for a flag until TransferControl is done?
            transferControl.transferCommand();
            // mainState = MainState::CommandSelection;
        } else if (buttonID == '*') {
            sendControl.sendMessage(0b10000);
            display.displayMessage(
                "\f\vTransfer\ncommand sent!",
                Display::Font::Mode8x8
            );
            hwlib::wait_ms(1'000);
            buttonChannel.clear();
            initCommandSelection();
        }
    }

    void initGameTimeInput() {
        message = "\f\vEnter game\ntime (1-15): ";
        minTime = 1;
        maxTime = 15;
        confGameTime = true;
    }

    void initCountdownInput() {
        message = "\f\vEnter count-\ndown (5-15): ";
        minTime = 5;
        maxTime = 15;
        confGameTime = false;
    }

    void initDistributeSettings() {
         display.displayMessage(
            "\t0005* - Send game   "
            "\n    settings",
            Display::Font::Mode8x8);
    }

    void initCommandSelection() {
        display.displayMessage(
            "\f\vA - Player\n    settings\n"
            "C - Start\n    new game\n"
            "D - Local\n    transfer\n"
            "* - Send\n    transfer",
            Display::Font::Mode8x8);
    }
};

#endif // INIT_CONTROL_HPP
