#ifndef RECEIVE_HIT_CONTROL_HPP
#define RECEIVE_HIT_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../entity/game_info.hpp"
#include "init_control.hpp"
#include "register_control.hpp"
#include "game_time_control.hpp"
#include "shot_control.hpp"
#include "../boundary/display.hpp"
#include "../boundary/buzzer.hpp"
#include <array>

class InitControl;
class RegisterControl;
class GameTimeControl;

/// \brief
/// Class ReceiveHitControl keeps trach of lives and controls the Buzzer
/// \details
/// This class is for keeping track of the lives and that you can not shoot yourself.
class ReceiveHitControl : public rtos::task<>{

enum state_t {INACTIVE, ACTIVE};

private:
    GameInfo& gameInfo;
    InitControl& initControl;
    RegisterControl& registerControl;
    ShotControl& shotControl;
    Display& display;
    Buzzer buzzer;
    GameTimeControl *gameTimeControl;

    rtos::channel<std::array<uint8_t, 2>, 10> hitReceivedChannel;
    rtos::flag gameOverFlag;
    rtos::flag startFlag;

    state_t state = INACTIVE;
public:
    /// \brief
    /// Constructor ReceiveHitControl
    /// \details
    /// This constructor has gameInfo, shotControl and Display by reference.
    /// The constructor will also make buzzer with pin d9.
    /// hitReceivedChannel, gameOverFlag and startFlag are created.
    ReceiveHitControl(
        GameInfo& gameInfo,
        InitControl& initControl,
        RegisterControl& registerControl,
        ShotControl& shotControl,
        Display& display
    );

    // scuffed workaround for missing a main controller GameControl
    void setGameTimeControl(GameTimeControl *timeControl);

    /// \brief
    /// hitReceived uint8_t playerID, uint8_t damage: void
    /// \details
    /// The function write to hitReceivedChannel that it is hit by a player with the PlayerId and the damage.
    void hitReceived(uint8_t playerID, uint8_t damage);

    /// \brief
    /// gameOver is to set the flag
    /// \details
    /// will set the flag gameOverFlag.
    void gameOver();

    /// \brief
    /// start is to set the flag
    /// \details
    /// will set the flag startFlag.
    void start();

    /// \brief
    /// main RegisterControl
    /// \details
    /// case INACTIVE
    ///     Waits for startFlag to be set.
    ///     if startFlag is set it will set lives to 100, playerID to gameInfo.getPlayerID and STATE to ACTIVE.
    /// case ACTIVE
    ///     hitReceivedChannel will read when there is a new hit, how higher the damage how longer to buzzer will go off.
    ///     if lives is below 0 it will send a gameOverFlag and the state go in to INACTIVE
    void main() override;
};


#endif // RECEIVE_HIT_CONTROL_HPP
