#include "hwlib.hpp"
#include "rtos.hpp"
#include "init_control.hpp"

InitControl::InitControl(
    GameInfo& gameInfo,
    SendControl& sendControl,
    TransferControl& transferControl,
    Keypad& keypad,
    Display& display
):
    task("init_control task"),
    gameInfo{gameInfo},
    sendControl{sendControl},
    transferControl{transferControl},
    keypad{keypad},
    display{display},
    buttonChannel(this, "button channel"),
    countdownClock(this, 1'000'000, "countdown clock"),
    startMenuFlag(this, "start menu flag (InitControl)")
{
    keypad.addButtonListener(this);
}

void InitControl::setGameTimeControl(GameTimeControl *timeControl) {
    gameTimeControl = timeControl;
}

void InitControl::setRegisterControl(RegisterControl *regControl) {
    registerControl = regControl;
}

void InitControl::startMenu() {
    startMenuFlag.set();
}

void InitControl::buttonPressed(int buttonID) {
    buttonChannel.write(buttonID);
}

void InitControl::startRegisterMenu() {
    registerControl->startMenu();
}

void InitControl::startGameTime(unsigned countdown) {
    gameTimeControl->start(countdown);
}

void InitControl::main() {
    mainState = MainState::Inactive;

    for (;;) {
        switch (mainState) {
        case MainState::Inactive           : inactive()           ; break;
        case MainState::ProcessInput       : processInput()       ; break;
        case MainState::DistributeSettings : distributeSettings() ; break;
        case MainState::GameInProgress     : gameInProgress()     ; break;
        case MainState::CommandSelection   : commandSelection()   ; break;
        default: break;
        }
    }
}
