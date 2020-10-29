#include "hwlib.hpp"
#include "rtos.hpp"
#include "send_control.hpp"


class ShotControl : public rtos::task<>{

enum state_t {INACTIVE, ACTIVE};

private:
    state_t state = INACTIVE;

    rtos::channel<int,10> buttonChannel;
    rtos::flag StartFlag;
    rtos::flag GameOverFlag;

    GameInfo gameInfo;
    Keypad keypad;
    Button trigger;
    
public:
    ShotControl():
    task( "ShotTask" ),
    buttonChannel( this, "buttonChannel" ),
    gameInfo( gameInfo& ),
    keypad( keypad ),
    trigger( trigger ),
    StartFlag( this, "StartFlag" ),
    GameOverFlag(this, "GameOverFlag")
    { 
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
                    wait(StartFlag);
                    weapon = gameInfo.getWeapon();
                    playerID = gameInfo.getPlayerID();
                    state = ACTIVE;
                case ACTIVE:
                    auto event = wait(buttonChannel + FlagGameOver);
                    if(event == buttonChannel){
                      button = buttonChannel.read();
                      if (button == '*' || button == 'T'){
                        sendControl.sendMessage((playerID<<5)+ weapon );
                        break;
                      }
                    }else if (event == FlagGameOver) {
                      state = INACTIVE;
                      break;
                    }
            }
        }
    }
};