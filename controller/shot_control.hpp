#ifndef SHOT_CONTROL_HPP
#define SHOT_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "send_control.hpp"
#include "../interface/button_listener.hpp"
#include "../entity/game_info.hpp"
#include "../boundary/trigger.hpp"
#include "../boundary/keypad.hpp"


class ShotControl : public rtos::task<>, public ButtonListener{

enum state_t {INACTIVE, ACTIVE};

private:
    SendControl& sendControl;
    GameInfo& gameInfo;
    Keypad& keypad;
    Trigger trigger;


    state_t state = INACTIVE;

    rtos::pool<int> buttonPool;
    rtos::flag buttonFlag;
    rtos::flag startFlag;
    rtos::flag gameOverFlag;
    rtos::timer timerWeaponCooldown;

    std::array<int, 8> cooldownForWeapon = {1'000,2'000,2'000,2'000,2'000,2'000,2'000,2'000}; //Only 2 weapons defined so far.


    
public:
    ShotControl(SendControl& sendControl, GameInfo& gameInfo, Keypad& keypad):
    task( "ShotTask" ),
    sendControl(sendControl),
    gameInfo(gameInfo),
    keypad(keypad),
    trigger('T', hwlib::target::pins::d6),
    buttonPool( "buttonPool" ),
    buttonFlag( this, "buttonFlag" ),
    startFlag( this, "startFlag" ),
    gameOverFlag(this, "gameOverFlag"),
    timerWeaponCooldown(this, "timerWeaponCooldown")
    {
        keypad.addButtonListener(this);
        trigger.addButtonListener(this);
        //Unsure if multiple classes should add the same keypad <- listener should make sure it doesnt have dupes, dont know if it does
    }

    void buttonPressed(int buttonID){
        buttonFlag.set();
        buttonPool.write(buttonID);
    }
    void start(){
        startFlag.set();
    }
    void gameOver(){
        gameOverFlag.set();
    }

private:
    void main(){
        unsigned int weapon;
        unsigned int playerID;
        unsigned int button;
        for(;;){
            switch(state){
                case INACTIVE:
                    wait(startFlag);
                    weapon = gameInfo.getWeapon();
                    playerID = gameInfo.getPlayerID();
                    state = ACTIVE;
                    break;
                case ACTIVE:
                    auto event = wait(buttonFlag + gameOverFlag);
                    if(event == buttonFlag){
                        wait(timerWeaponCooldown);
                        button = buttonPool.read();
                        if(button == '*' || button == 'T'){
                            sendControl.sendMessage((playerID<<5)+ weapon );
                            timerWeaponCooldown.set(cooldownForWeapon[weapon]*1'000);
                        }
                    }else if(event == gameOverFlag){
                        state = INACTIVE;
                    }
                    break;
            }
        }
    }
    //verrandering
};

#endif // SHOT_CONTROL_HPP
