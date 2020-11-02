#ifndef RECEIVE_HIT_CONTROL_HPP
#define RECEIVE_HIT_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../entity/game_info.hpp"
#include "shot_control.hpp"
#include "../boundary/display.hpp"
#include "../boundary/buzzer.hpp"
#include <array>

class ReceiveHitControl : public rtos::task<>{

enum state_t {INACTIVE, ACTIVE};

private:
    GameInfo& gameInfo;
    ShotControl& shotControl;
    Display& display;
    Buzzer buzzer;

    rtos::channel<std::array<uint8_t, 2>, 10> hitReceivedChannel;
    rtos::flag gameOverFlag;
    rtos::flag startFlag;

    state_t state = INACTIVE;
public:
    ReceiveHitControl(
        GameInfo& gameInfo,
        ShotControl& shotControl,
        Display& display
    ):
        task("ReceiveHitControl"),
        gameInfo(gameInfo),
        shotControl(shotControl),
        display(display),
        buzzer(hwlib::target::pins::d9),
        hitReceivedChannel(this, "hitReceivedChannel"),
        gameOverFlag(this, "gameOverFlag"),
        startFlag(this, "startFlag")
    {}

    void hitReceived(uint8_t playerID, uint8_t damage){
        hitReceivedChannel.write({playerID, damage});
    }
    void gameOver(){
        gameOverFlag.set();
    }
    void start(){
        startFlag.set();
    }
private:
    void main(){
        int lives = 0;
        uint8_t playerID = 0;
        std::array<uint8_t, 2> hit = {0,0};
        for(;;){
            switch(state){
                case INACTIVE:
                    wait( startFlag );
                    lives = 100;
                    playerID = gameInfo.getPlayerID();
                    state = ACTIVE;
                    break;
                case ACTIVE:
                    hit = hitReceivedChannel.read();
                    if(hit[0]==playerID){
                        break;
                    }else if((lives - hit[1]) <= 0){
                        gameOverFlag.set();
                        display.displayMessage("\t0002Lives:\t00003", 0);
                        buzzer.playSound(1);
                        hwlib::wait_ms((hit[1]/20)*1000);
                        buzzer.playSound(0);
                        gameInfo.registerHit(hit[0], lives);
                        state = INACTIVE;
                        break;
                    }else{
                        lives -= hit[1];
                        gameInfo.registerHit(hit[0], hit[1]);
                        display.displayMessage("\t0002Lives:\t0003", lives);
                        buzzer.playSound(1);
                        hwlib::wait_ms((hit[1]/20)*1000);
                        buzzer.playSound(0);
                        break;
                    }
            }
        }
    }
};


#endif // RECEIVE_HIT_CONTROL_HPP
