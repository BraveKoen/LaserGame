#include "hwlib.hpp"
#include "rtos.hpp"
#include "SendControl.hpp"


class ShotControl : public rtos::task<>{

enum state_t {INACTIVE, ACTIVE};

private:
    state_t state = INACTIVE;

    rtos::channel<int,10> buttonChannel;
    rtos::flag flagStart;

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
    enable_flag( this, "flagStart" )
    {}

    void buttonPressed(int buttonID){
        buttonChannel.write(buttonID);
    }
    void setFlag(){
        flagStart.set();
    }

private:
    void main(){
        unsigned int weapon;
        unsigned int playerID;
        for(;;){
            switch(state){
                case INACTIVE:
                    wait(flagStart);
                    weapon = gameInfo.getWeapon();
                    playerID = gameInfo.getPlayerID();
                    state = ACTIVE;
                case ACTIVE:
                    button = buttonChannel.read();
                    if(button == '*' || button == 'T'){
                        sendControl.sendMessage(0b0+(playerID<<10)+(weapon<<5)+(playerID ^weapon));
                    }
            }
        }
    }
};