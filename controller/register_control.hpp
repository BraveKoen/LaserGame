#ifndef REGISTER_CONTROL_HPP
#define REGISTER_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../entity/game_info.hpp"
#include "../boundary/display.hpp"
#include "../boundary/keypad.hpp"
#include "../interface/button_listener.hpp"

class RegisterControl : public rtos::task<>, public ButtonListener{

enum states {INACTIVE, CONFIGURING};
enum substates {DEFAULT, ENTERINGPLAYERNUMBER, ENTERINGWEAPONTYPE};

private:
    states state = INACTIVE;
    substates substate = DEFAULT;

    GameInfo& gameInfo;
    Display& display;
    Keypad& keypad;
    rtos::channel<uint8_t, 10> buttonChannel;
    rtos::pool<int> gameTimePool;
    rtos::flag gameTimeFlag;

public:
RegisterControl(GameInfo& gameInfo, Display& display, Keypad& keypad):
    task("RegisterTask"),
    gameInfo(gameInfo),
    display(display),
    keypad(keypad),
    buttonChannel(this, "buttonChannel"),
    gameTimePool("gameTimePool"),
    gameTimeFlag(this,"gameTimeFlag")
    {}

    void gameTime(int time){
        gameInfo.setTime(time);
    }
    void buttonPressed(int buttonID) override { // change type of buttonID
        buttonChannel.write(buttonID);
    }

private:
    void main(){
        uint8_t button = 0;
        uint8_t playerID = 0;
        uint8_t weaponType = 0;
        for(;;){
            switch(state){
                case INACTIVE: {
                    auto event = wait(gameTimeFlag + buttonChannel);
                    if(event == gameTimeFlag){
                        gameInfo.setTime(gameTimePool.read());
                    }else if(event == buttonChannel){
                        button = buttonChannel.read();
                        state = CONFIGURING;
                    }
                    break;
                }
                case CONFIGURING:
                    switch(substate){
                        case DEFAULT:
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
                        case ENTERINGPLAYERNUMBER:
                            display.displayMessage("\t0000playerID: ", playerID);
                            button = buttonChannel.read();
                            if(button == '#'){
                                gameInfo.setPlayerID(playerID);
                                display.clear();
                                state = INACTIVE;
                                substate = DEFAULT;
                            }else if(button >= '0' && button <= '9'){
                                playerID *= 10;
                                playerID += (button -'0');
                                if(playerID > 31){
                                    playerID = 1;
                                    display.displayMessage("\t0000playerID reset ");
                                    hwlib::wait_ms(1000);
                                }
                            }
                            break;
                        case ENTERINGWEAPONTYPE:
                            display.displayMessage("\t0000Weapon type: ", weaponType);
                            button = buttonChannel.read();
                            if(button == '#'){
                                gameInfo.setPlayerID(playerID);
                                display.clear();
                                state = INACTIVE;
                                substate = DEFAULT;
                            }else if(button >= '0' && button <= '8'){
                                weaponType = ((button-'0')-1);
                            }
                            break;
                    }
                    break;
            }
        }
    }
};

#endif // REGISTER_CONTROL_HPP
