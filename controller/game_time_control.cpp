#include "hwlib.hpp"
#include "rtos.hpp"
#include "game_time_control.hpp"

GameTimeControl::GameTimeControl(
    GameInfo& gameInfo,
    InitControl& initControl,
    RegisterControl& registerControl,
    ShotControl& shotControl,
    ReceiveHitControl& receiveHitControl,
    Display& display
):
    task("GameTimeTask"),
    gameInfo(gameInfo),
    initControl(initControl),
    registerControl(registerControl),
    shotControl(shotControl),
    receiveHitControl(receiveHitControl),
    display(display),
    countDownPool("countDownPool"),
    countDownFlag(this, "countDownFlag"),
    gameOverFlag(this, "game over flag (GameTimeControl)"),
    clock_10s(this, 10'000'000, "1s clock")
{
    initControl.setGameTimeControl(this);
    receiveHitControl.setGameTimeControl(this);
}

void GameTimeControl::gameOver() {
    gameOverFlag.set();
}

void GameTimeControl::start(unsigned int countdown = 1){
    countDownPool.write(countdown);
    countDownFlag.set();
}

void GameTimeControl::main(){
    int countdown;
    int gameTime;

    for(;;){
        switch(state){
            case INACTIVE: 
                wait(countDownFlag);
                countdown = countDownPool.read();
                gameTime = gameInfo.getTime() * 60;
                display.clear();
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
                        display.displayMessage("\vStart:\t00010", countdown);
                    }else{
                        display.displayMessage("\vStart:\t0001", countdown); 
                    }
                    countdown--;
                    hwlib::wait_ms(1000);
                    break;
                }
            case GAMETIME: {
                auto event = wait(clock_10s + gameOverFlag);
                if(event == clock_10s){
                    if(gameTime > 10){
                        // hwlib::wait_ms(10000);
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
                        if (gameInfo.getPlayerID() == 0) {
                            initControl.startMenu();
                        } else {
                            registerControl.startMenu();
                        }
                        shotControl.gameOver();
                        receiveHitControl.gameOver();
                        state = INACTIVE;
                        break;
                    }
                } else if(event == gameOverFlag){
                    state = INACTIVE;
                    break;
                }
            }
        }
    }
}
