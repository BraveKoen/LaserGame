#include "hwlib.hpp"
#include "rtos.hpp"
#include "game_time_control.hpp"

GameTimeControl::GameTimeControl(
    GameInfo& gameInfo,
    InitControl& initControl,
    ShotControl& shotControl,
    ReceiveHitControl& receiveHitControl,
    Display& display
):
    task("GameTimeTask"),
    gameInfo(gameInfo),
    initControl(initControl),
    shotControl(shotControl),
    receiveHitControl(receiveHitControl),
    display(display),
    countDownPool("countDownPool"),
    countDownFlag(this, "countDownFlag"),
    clock_1s(this, 1'000'000, "1s clock")
{
    initControl.setGameTimeControl(this);
}

void GameTimeControl::start(unsigned int countdown = 1){
    countDownPool.write(countdown);
    countDownFlag.set();
}

void GameTimeControl::main(){
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
                    display.displayMessage("\f\vGO!");
                    shotControl.start();
                    receiveHitControl.start();
                    state = GAMETIME;
                    break;
                }else{
                    if(countdown<10){
                        display.displayMessage("\f\vStart:\t00010", countdown);
                    }else{
                        display.displayMessage("\f\vStart:\t0001", countdown); 
                    }
                    countdown--;
                    hwlib::wait_ms(1000);
                    break;
                }
            case GAMETIME:
                if(gameTime > 1){
                    hwlib::wait_ms(10000);
                    gameTime-=10;
                    if(gameTime/60<10){
                        display.displayMessage("\vTimer:\t00010", gameTime/60);
                    }else{
                        display.displayMessage("\vTimer:\t0001", gameTime/60);
                    }
                    if(gameTime%60<10){
                        display.displayMessage(":0", gameTime%60);
                    }else{
                        display.displayMessage(":", gameTime%60); 
                    }
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
