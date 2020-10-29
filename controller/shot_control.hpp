#include "hwlib.hpp"
#include "rtos.hpp"
#include "send_control.hpp"


class ShotControl : public rtos::task<>{

enum state_t {INACTIVE, ACTIVE};

private:
    state_t state = INACTIVE;

    rtos::channel<int,10> buttonChannel;
    rtos::flag startFlag;
    rtos::flag gameOverFlag;

    SendControl& sendControl;
    GameInfo& gameInfo;
    Button& keypad;
    Button trigger;
    
public:
    ShotControl(Button& keypad, ButtonHandler& handler, GameInfo& gameInfo, SendControl& sendControl):
    task( "ShotTask" ),
    sendControl(sendControl),
    gameInfo(gameInfo),
    keypad(keypad),
    trigger(),
    buttonChannel( this, "buttonChannel" ),
    startFlag( this, "StartFlag" ),
    gameOverFlag(this, "GameOverFlag")
    {
        keypad.addButtonListener(this);
        trigger.addButtonListener(this);
        //Unsure if multiple classes should add the same keypad
        handler.addButton(keypad);
        handler.addButton(trigger);

    }

    void buttonPressed(int buttonID){
        buttonChannel.write(buttonID);
    }
    void start(){
        StartFlag.set();
    }
    void gameOver(){
        GameOverFlag.set();
    }

private:
    void main(){
        unsigned int weapon;
        unsigned int playerID;
        for(;;){
            switch(state){
                case INACTIVE:
                    wait(startFlag);
                    weapon = gameInfo.getWeapon();
                    playerID = gameInfo.getPlayerID();
                    state = ACTIVE;
                case ACTIVE:
                    auto event = wait(buttonChannel + gameOverFlag);
                    if(event == buttonChannel){
                      button = buttonChannel.read();
                      if (button == '*' || button == 'T'){
                        sendControl.sendMessage((playerID<<5)+ weapon );
                        break;
                      }
                    } else if (event == gameOverFlag) {
                      state = INACTIVE;
                      break;
                    }
            }
        }
    }
    //verrandering
};