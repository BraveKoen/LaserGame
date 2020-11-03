#ifndef GAME_TIME_CONTROL_HPP
#define GAME_TIME_CONTROL_HPP

#include "../entity/game_info.hpp"
//#include "init_control.hpp"
#include "hwlib.hpp"
#include "rtos.hpp"
#include "shot_control.hpp"
#include "receive_hit_control.hpp"

class GameTimeControl : public rtos::task<>{
    enum state_t { INACTIVE, COUNTDOWN, GAMETIME };

private:
    state_t state = INACTIVE;

    rtos::pool<int> countDownPool;
    rtos::flag countDownFlag;
    rtos::clock clock_1s;

    GameInfo& gameInfo;
    // InitControl& initControl;
    ShotControl& shotControl;
    ReceiveHitControl& receiveHitControl;
    Display& display;

public:
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
    clock_1s(this, 1'000'000, "1s clock"), 
    gameInfo(gameInfo),
    // initControl(initControl),
    shotControl(shotControl),
    receiveHitControl(receiveHitControl),
    display(display)
    {}

    void start(unsigned int countdown = 1){
        countDownPool.write(countdown);
        countDownFlag.set();
    }

private:
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
                        receiveHitControl.start();
                        state = GAMETIME;
                        break;
                    }else{
                        display.displayMessage("\t0000Start:\t0001", countdown); 
                        countdown--;
                        hwlib::wait_ms(1000);
                        break;
                    }
                case GAMETIME:
                    if(gameTime > 1){
                        hwlib::wait_ms(1000);
                        gameTime--;
                        display.displayMessage("\t0000Timer:\t0001", gameTime/60); 
                        display.displayMessage(":", gameTime%60); 
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