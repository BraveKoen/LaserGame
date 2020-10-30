#ifndef INIT_CONTROL_HPP
#define INIT_CONTROL_HPP

#include "game_info.hpp"
#include "init_control.hpp"
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

    GameInfo gameInfo;
    InitControl initControl;
    ShotControl shotControl;
    ReceiveHitControl receiveHitControl;
    Display& display;

public:
    GameTimeControl(GameInfo& gameInfo, InitControl& initControl, ShotControl& shotControl, ReceiveHitControl& receiveHitControl, Display& display):
    task("GameTimeTask"),
    countDownPool("countDownPool"),
    countDownFlag(this, "countDownFlag"),
    gameInfo(gameInfo),
    initControl(initControl),
    shotControl(shotControl),
    receiveHitControl(receiveHitControl),
    display(display){}

    void start(unsigned int countdown = 1){
        countDownPool.write(countdown);
        countDownFlag.set();
    }

private:
    void main(){
        unsigned int countdown;
        unsigned int gameTime;
        unsigned int currentTime;

        currentTime = hwlib::now_us();
        countdown = countDownPool.read();
        gameTime = gameInfo.getTime();
        for(;;){
            switch(state){
                case INACTIVE: 
                    wait(countDownFlag);
                    state = COUNTDOWN;  
                    break;
                case COUNTDOWN:
                    if(countdown <= 0){
                        state = GAMETIME;
                        break;
                    }else{
                        display.displayMessage("\t0000Start:\t0001"<<countdown);
                        countdown--;
                        hwlib::wait_ms(1'000);
                        break;
                    }
                case GAMETIME:
                    display.displayMessage("\t0000GO!");
                    if(gameTime > 1){
                        hwlib::wait_ms(1'000);
                        gameTime--;
                        display.displayMessage("\t0000Time:\t0001" << gameTime);
                        break;
                    }else{
                        initControl.gameOver();
                        shotControl.gameOver();
                        receiveHitControl.gameOver();
                        state = INACTIVE;
                        break;
                    }
            }
        }
    }
};