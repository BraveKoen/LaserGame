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

    void start(unsigned int countdown);

    void main() override;
};

#endif //GAME_TIME_CONTROL
