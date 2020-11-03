#ifndef GAME_TIME_CONTROL_HPP
#define GAME_TIME_CONTROL_HPP

#include "../entity/game_info.hpp"
//#include "init_control.hpp"
#include "hwlib.hpp"
#include "rtos.hpp"
#include "shot_control.hpp"
#include "receive_hit_control.hpp"

/// \brief
/// Class GameTimeControl
/// \details
/// This class is used for keeping track of time.
class GameTimeControl : public rtos::task<>{
    enum state_t { INACTIVE, COUNTDOWN, GAMETIME };

private:
    state_t state = INACTIVE;

    rtos::pool<int> countDownPool;
    rtos::flag countDownFlag;

    GameInfo& gameInfo;
    // InitControl& initControl;
    ShotControl& shotControl;
    ReceiveHitControl& receiveHitControl;
    Display& display;

public:
/// \brief
/// Constructor GameTimeControl.
/// \details
/// the Constructor expects GameInfo, ShotControl, ReceiveHitControl and Display all by reference.
/// countDownPool and countDownFlag are created.
    GameTimeControl(
        GameInfo& gameInfo,
        // InitControl& initControl,
        ShotControl& shotControl,
        ReceiveHitControl& receiveHitControl,
        Display& display
    ):
    task("GameTimeTask"),
    countDownPool("countDownPool"),
    countDownFlag(this, "countDownFlag"),
    gameInfo(gameInfo),
    // initControl(initControl),
    shotControl(shotControl),
    receiveHitControl(receiveHitControl),
    display(display)
    {}
/// \brief
/// Function start int countdown with a default 1: void
/// \details
/// Writes the countdown to the pool and will set the countDownFlag
    void start(unsigned int countdown = 1){
        countDownPool.write(countdown);
        countDownFlag.set();
    }

private:
    /// \brief
    /// main GameTimeControl
    /// \details
    /// there are three states INACTIVE, COUNTDOWN and GAMETIME
    /// case INACTIVE
    ///     Waits for startFlag to be set.
    ///     if startFlag is set it will set lives to 100, playerID to gameInfo.getPlayerID and STATE to ACTIVE.
    /// case ACTIVE
    ///     hitReceivedChannel will read when there is a new hit, how higher the damage how longer to buzzer will go off.
    ///     if lives is below 0 it will send a gameOverFlag and the state go in to INACTIVE
    void main(){
        unsigned int countdown;
        unsigned int gameTime;

        for(;;){
            switch(state){
                case INACTIVE: 
                    wait(countDownFlag);
                    countdown = countDownPool.read();
                    gameTime = gameInfo.getTime() * 60;
                    state = COUNTDOWN;  
                    break;
                case COUNTDOWN:
                    if(countdown <= 0){
                        display.displayMessage("\t0000GO!");
                        shotControl.start();
                        receiveHitControl.start();
                        state = GAMETIME;
                        break;
                    }else{
                        display.displayMessage("\t0000Start:\t0001", countdown); 
                        countdown--;
                        hwlib::wait_ms(1'000);
                        break;
                    }
                case GAMETIME:
                    if(gameTime > 1){
                        hwlib::wait_ms(1'000);
                        gameTime--;
                        display.displayMessage("\t0000Time:\t0001", gameTime);
                        break;
                    }else{
                        // initControl.gameOver();
                        shotControl.gameOver();
                        receiveHitControl.gameOver();
                        state = INACTIVE;
                        break;
                    }
            }
        }
    }
};
#endif //GAME_TIME_CONTROL