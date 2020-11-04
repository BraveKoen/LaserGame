#ifndef GAME_TIME_CONTROL_HPP
#define GAME_TIME_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../entity/game_info.hpp"
#include "init_control.hpp"
#include "register_control.hpp"
#include "shot_control.hpp"
#include "receive_hit_control.hpp"
#include "../boundary/display.hpp"

class InitControl;
class ReceiveHitControl;

/// \brief
/// class GameTimeControl controls the gametime.
/// \details
/// class will display countdown and gametime and will count down both
class GameTimeControl : public rtos::task<>{
    enum state_t { INACTIVE, COUNTDOWN, GAMETIME };
private:
    state_t state = INACTIVE;

    GameInfo& gameInfo;
    InitControl& initControl;
    RegisterControl& registerControl;
    ShotControl& shotControl;
    ReceiveHitControl& receiveHitControl;
    Display& display;

    rtos::pool<int> countDownPool;
    rtos::flag countDownFlag;
    rtos::flag gameOverFlag;
    rtos::clock clock_10s;
public:
    GameTimeControl(
        GameInfo& gameInfo,
        InitControl& initControl,
        RegisterControl& registerControl,
        ShotControl& shotControl,
        ReceiveHitControl& receiveHitControl,
        Display& display
    );

    /// \brief
    /// gameOver is to set the flag
    /// \details
    /// will set the flag gameOverFlag.
    void gameOver();

    /// \brief
    /// start is to start the countdown period
    /// \details
    /// will set put countdown in pool and set flag
    void start(unsigned int countdown);

    /// \brief
    /// this main will switch between counting down gametime and cooldown or nothing
    /// \details
    /// inactive state waits for countdown pool, after wich state countdown will start counting down.
    /// after the countdown the game time itself will be displayed.
    /// when time is up, function will stop all other necessary classes.
    void main() override;
};

#endif //GAME_TIME_CONTROL
