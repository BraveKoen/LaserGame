#ifndef REGISTER_CONTROL_HPP
#define REGISTER_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "init_control.hpp"
#include "../entity/game_info.hpp"
#include "../boundary/display.hpp"
#include "../boundary/keypad.hpp"
#include "../interface/button_listener.hpp"

class InitControl;

/// \brief
/// Class RegisterControl player input, PlayerID and WeaponType 
/// \details
/// This class will send PlayerID and WeaponID to the class GameInfo 
class RegisterControl : public rtos::task<>, public ButtonListener{

enum states {INACTIVE, MENUSELECT, CONFIGURING};
enum substates {DEFAULT, ENTERINGPLAYERNUMBER, ENTERINGWEAPONTYPE};

private:
    states state = MENUSELECT;
    substates substate = DEFAULT;

    GameInfo& gameInfo;
    InitControl& initControl;
    Display& display;
    Keypad& keypad;

    rtos::channel<uint8_t, 10> buttonChannel;
    rtos::pool<int> gameTimePool;
    rtos::flag gameTimeFlag;
    rtos::flag startMenuFlag;
public:
    /// \brief
    /// Constructor for class RegisterControl
    /// \details
    /// This constructor has gameInfo, display and keypad by reference. 
    /// Also there is a gameTimePool, gameTimeFlag and a buttonChannel created.
    RegisterControl(
        GameInfo& gameInfo,
        InitControl& initControl,
        Display& display,
        Keypad& keypad
    );

    /// \brief
    /// gameTime function void
    /// \details
    /// This function set the gametime in gameInfo enety
    void gameTime(int time);

    void buttonPressed(int buttonID) override;

    void startMenu();

    /// \brief
    /// main RegisterControl
    /// \details
    /// case INACTIVE
    ///     Waits for gameFlagTime or buttonChannel to be set.
    ///     if buttonChannel is pressed it will switch to case CONFIGURING
    /// case CONFIGURING
    ///     Got three substatus DEFAULT, ENTERINGPLAYERNUMBER and ENTERINGWEAPONTYPE
    ///     case DEFAULT
    ///         waits for button input and will display a message depending which button is pressed.
    ///     case ENTERINGPLAYERNUMBER
    ///         Player input for playerID it can not be higher then 31 or lower then 0.
    ///         if * is pressed on the keypad the case will send the playerID to GameInfo
    ///     case ENTERINGWEAPONTYPE
    ///         Player can choose between weapon 1 to 8.
    ///         if # is pressed the given weapontype will be send to gameInfo.
    void main() override;
};

#endif // REGISTER_CONTROL_HPP
