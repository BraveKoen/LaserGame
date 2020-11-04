#include "hwlib.hpp"
#include "rtos.hpp"
#include "register_control.hpp"

RegisterControl::RegisterControl(
    GameInfo& gameInfo,
    InitControl& initControl,
    Display& display,
    Keypad& keypad
):
    task("RegisterTask"),
    gameInfo(gameInfo),
    initControl(initControl),
    display(display),
    keypad(keypad),
    buttonChannel(this, "buttonChannel"),
    gameTimeFlag(this,"gameTimeFlag"),
    startMenuFlag(this, "start menu flag (RegisterControl)")
{
    keypad.addButtonListener(this);
    initControl.setRegisterControl(this);
}

void RegisterControl::gameTime(int time){
    gameInfo.setTime(time);
    gameTimeFlag.set();
}

// change type of buttonID
void RegisterControl::buttonPressed(int buttonID) {
    buttonChannel.write(buttonID);
}

void RegisterControl::startMenu() {
    startMenuFlag.set();
}

void RegisterControl::main(){
    uint8_t button = 0;
    uint8_t playerID = gameInfo.getPlayerID();
    uint8_t weaponType = 0;
    for(;;){
        switch(state){
            case INACTIVE:
                wait(startMenuFlag);
                substate = DEFAULT;
                state = MENUSELECT;
                break;
            case MENUSELECT: {
                char message[]{
                    "\fA - Enter\n    playerID\n"
                    "B - Enter\n    weapon type\n"
                    "C - Start\n    new game"
                };
                message[49] = playerID == 0 ? '\n' : '\0';
                display.displayMessage(
                    message,
                    Display::Font::Mode8x8
                );
                auto event = wait(gameTimeFlag + buttonChannel);
                if(event == gameTimeFlag){
                    state = INACTIVE;
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
                            display.displayMessage(
                                "\f\vEnter player\nnumber (0-31)", 
                                Display::Font::Mode8x8
                            );
                            substate = ENTERINGPLAYERNUMBER;
                        }else if(button=='B'){
                            display.displayMessage(
                                "\f\vEnter weapon\ntype (1-8)",
                                Display::Font::Mode8x8
                            );
                            substate = ENTERINGWEAPONTYPE;
                        }else if(button=='C' and playerID == 0){
                            initControl.startMenu();
                            state = INACTIVE;
                        }else{
                            state = MENUSELECT;
                        }
                        break;
                    case ENTERINGPLAYERNUMBER:
                        display.displayMessage(
                            "\t0003Player ID: ", 
                            playerID, 
                            Display::Font::Mode8x8
                        );
                        button = buttonChannel.read();
                        if(button == '#'){
                            gameInfo.setPlayerID(playerID);
                            display.clear();
                            state = MENUSELECT;
                            substate = DEFAULT;
                        }else if(button >= '0' && button <= '9'){
                            playerID *= 10;
                            playerID += (button - '0');
                            if(playerID > 31 || playerID < 0){
                                playerID = 0;
                                display.displayMessage(
                                    "\rReset Player ID",
                                    Display::Font::Mode8x8
                                );
                                hwlib::wait_ms(1'000);
                                display.displayMessage(
                                    "\r
                                    ",
                                    Display::Font::Mode8x8
                                );
                            }
                        }
                        break;
                    case ENTERINGWEAPONTYPE:
                        display.displayMessage(
                            "\t0003Weapon type: ",
                            weaponType,
                            Display::Font::Mode8x8
                        );
                        button = buttonChannel.read();
                        if(button == '#'){
                            gameInfo.setWeapon(weaponType - 1);
                            display.clear();
                            state = MENUSELECT;
                            substate = DEFAULT;
                        }else if(button >= '0' && button <= '8'){
                            weaponType = button - '0';
                        }
                        break;
                }
                break;
        }
    }
}
