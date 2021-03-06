#include "hwlib.hpp"
#include "rtos.hpp"
#include "receive_hit_control.hpp"

ReceiveHitControl::ReceiveHitControl(
    GameInfo& gameInfo,
    InitControl& initControl,
    RegisterControl& registerControl,
    ShotControl& shotControl,
    Display& display
):
    task("ReceiveHitControl"),
    gameInfo(gameInfo),
    initControl(initControl),
    registerControl(registerControl),
    shotControl(shotControl),
    display(display),
    buzzer(hwlib::target::pins::d9),
    hitReceivedChannel(this, "hitReceivedChannel"),
    gameOverFlag(this, "gameOverFlag"),
    startFlag(this, "startFlag")
{} 

void ReceiveHitControl::setGameTimeControl(GameTimeControl *timeControl) {
    gameTimeControl = timeControl;
}

void ReceiveHitControl::hitReceived(uint8_t playerID, uint8_t damage){
    hitReceivedChannel.write({playerID, damage});
}

void ReceiveHitControl::gameOver(){
    gameOverFlag.set();
}

void ReceiveHitControl::start(){
    startFlag.set();
}

void ReceiveHitControl::main(){
    int lives = 0;
    uint8_t playerID = 0;
    std::array<uint8_t, 2> hit = {0,0};
    for(;;){
        //hwlib::cout<<lives;
        switch(state){
            case INACTIVE:
                wait(startFlag);
                lives = 100;
                display.displayMessage("\t002Lives:\t0003", lives);
                playerID = gameInfo.getPlayerID();
                state = ACTIVE;
                break;
            case ACTIVE:
                auto event = wait(hitReceivedChannel + gameOverFlag);
                if(event == hitReceivedChannel){
                    hit = hitReceivedChannel.read();
                    if((lives - hit[1]) <= 0){
                        display.displayMessage("\t0002Lives:\t00030",0);
                        buzzer.hitSound();
                        gameInfo.registerHit(hit[0], lives);
                        gameTimeControl->gameOver();
                        shotControl.gameOver();
                        if (gameInfo.getPlayerID() == 0) {
                            initControl.startMenu();
                        } else {
                            registerControl.startMenu();
                        }
                        state = INACTIVE;
                        break;
                    }else{
                        lives -= hit[1];
                        gameInfo.registerHit(hit[0], hit[1]);
                        if(lives<10){
                            display.displayMessage("\t0002Lives:\t000300", lives);
                        }else if(lives<100){
                            display.displayMessage("\t0002Lives:\t00030", lives);
                        }else{
                            display.displayMessage("\t0002Lives:\t0003", lives);
                        }
                        buzzer.hitSound();
                        break;
                    }
                }else if(state == gameOverFlag){
                    state = INACTIVE;
                    break;
                }
            break;
        }
    }
}
