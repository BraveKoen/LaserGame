#include "hwlib.hpp"
#include "rtos.hpp"
#include "receive_hit_control.hpp"

ReceiveHitControl::ReceiveHitControl(
    GameInfo& gameInfo,
    InitControl& initControl,
    ShotControl& shotControl,
    Display& display
):
    task("ReceiveHitControl"),
    gameInfo(gameInfo),
    initControl(initControl),
    shotControl(shotControl),
    display(display),
    buzzer(hwlib::target::pins::d9),
    hitReceivedChannel(this, "hitReceivedChannel"),
    gameOverFlag(this, "gameOverFlag"),
    startFlag(this, "startFlag")
{} 

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
                hit = hitReceivedChannel.read();
                if(hit[0]==playerID){
                    break;
                }else if((lives - hit[1]) <= 0){
                    // gameOverFlag.set(); // uhm... what??
                    shotControl.gameOver();
                    initControl.gameOver();
                    display.displayMessage("\t0002Lives:\t00030",0);
                    buzzer.playSound(1);
                    hwlib::wait_ms((hit[1]/20)*1000);
                    buzzer.playSound(0);
                    gameInfo.registerHit(hit[0], lives);
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
                    buzzer.playSound(1);
                    hwlib::wait_ms((hit[1]/20)*1000);
                    buzzer.playSound(0);
                    break;
                }
        }
    }
}
