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
    void enableFlag(){
        flagStart.set();
    }

private:
    void main(){
        for(;;){
            switch(state){
                case INACTIVE:
                    wait(flagStart);
                    unsigned int weapon = gameInfo.getWeapon();
                    unsigned int playerID = gameInfo.getPlayerID();

};