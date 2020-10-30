#ifndef REGISTER_CONTROL_HPP
#define REGISTER_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../entity/game_info.hpp"
#include "../boundary/display.hpp"
#include "../boundary/keypad.hpp"
#include "../interface/button_listener.hpp"

class RegisterControl : public rtos::task<>, public ButtonListener{

enum state_t {INACTIVE, CONFIGURING};
enum state_sub {ENTERINGPLAYERNUMBER, ENTERINGWEAPONTYPE};

private:
    state_t state = INACTIVE;
    state_sub substate = ENTERINGPLAYERNUMBER;
    
    rtos::channel<uint8_t buttonID, 10> buttonChannel;
    rtos::pool<int time> gameTimePool;
    rtos::flag gameTimeFlag;

    GameInfo& gameInfo;
    Display& display;
    Keypad& keypad;


public:
RegisterControl(GameInfo& gameInfo, Display& display, Keypad& keypad):
    task("RegisterTask"),
    buttonChannel( this, "buttonChannel"),
    gameTimePool("gameTimePool"),
    gameTimeFlag(this,"gameTimeFlag"),
    gameInfo(gameInfo),
    display(display),
    keypad(keypad)
    {}

    void gameTime(int time){
        gameInfo.setTime(time);
    }
    void buttonPressed(uint8_t buttonID){
        buttonChannel.write(buttonID);
    }

private:
    void main(){
        uint8_t button = 0;
        uint8_t playerID = 0;
        uint8_t weaponType = 0;
        for(;;){
            switch(state){
                case INACTIVE:
                    auto event = wait(gameTimeFlag + buttonChannel);
                    if(event == gameTimeFlag){
                        gameInfo.setTime(gameTimePool.read());
                    }else if(event == buttonChannel){
                        button = buttonChannel.read();
                        state = CONFIGURING;
                    }
                    break;
                case CONFIGURING:
                    if(button=='A'){
                        display.displayMessage("\t0000Enter Player Number");
                        substate = ENTERINGPLAYERNUMBER;
                    }else if(button== 'B'){
                        display.displayMessage("\t0000Weapon Type\t0001(1-8): ");
                        substate = ENTERINGWEAPONTYPE;
                    }else{
                        display.clear();
                    }
                    break;

                    switch(substate){
                        case ENTERINGPLAYERNUMBER:
                            display.displayMessage("\t0000playerID: " << playerID);
                            button = buttonChannel.read();
                            if(button == '#'){
                                gameinfo.setPlayerID(playerID);
                                display.clear();
                                state = INACTIVE;
                            }else if(button >= '0' && button <= '9'){
                                playerID *= 10;
                                playerID += (button -'0');
                                if(playerID > 31){
                                    playerID = 0;
                                    display.displayMessage("\t0000playerID reset ");
                                    hwlib::wait_ms(1000);
                                }
                            }
                            break;
                        case ENTERINGWEAPONTYPE:
                            display.displayMessage("\t0000Weapon type: "<< weaponType);
                            botton = buttonChannel.read();
                            if(button == '#'){
                                gameInfo.setPlayerID(playerID);
                                display.clear();
                                state = INACTIVE;
                            }else if(button >= '0' && button <= '8'){
                                weaponType = ((button-'0')-1);
                            }
                            break;

                    }
            }

        }
    }

};

#endif // REGISTER_CONTROL_HPP
