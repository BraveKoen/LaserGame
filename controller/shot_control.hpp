#ifndef SHOT_CONTROL_HPP
#define SHOT_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "send_control.hpp"
#include "../interface/button_listener.hpp"
#include "../entity/game_info.hpp"
#include "../boundary/trigger.hpp"
#include "../boundary/keypad.hpp"

/// \brief
/// Class ShotControl, class makes players able to shoot at others. 
/// \details
/// When at play this class will wait for the correct buttons to be pressed and then it will shoot shot data.
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
    /// \brief
    /// This constructor makes new instance of ShotControl, gives it a static task. 
    /// \details
    /// The constructor requests a SendControl, GameInfo and Keypad.
    /// It also initializes a pool and three flags, each with fixed tasks.
    /// A ButtonListener will be added to both the keypad and the trigger.
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
    /// \brief
    /// Public function buttonPressed requests an int and puts it in a pool 
    /// \details
    /// When called, this function sets the buttonFlag and puts the buttonID into the buttonPool
    void buttonPressed(int buttonID){
        buttonFlag.set();
        buttonPool.write(buttonID);
    }
    /// \brief
    /// Public function starts sets the startFlag 
    /// \details
    /// Nothing special, this function sets the sart flag, making other classes able to start up this one.
    void start(){
        startFlag.set();
    }
    /// \brief
    /// Public function gameOver sets the gameOverFlag. 
    /// \details
    /// Simple function that lets other classes set the gameOverFlag.
    void gameOver(){
        gameOverFlag.set();
    }

private:
    /// \brief
    /// Private function main makes the players able to shoot.
    /// \details
    /// case INACTIVE
    ///     This case waits for the startFlag,
    ///     saves the weaponType and playerID
    ///     and then sets the state to ACTIVE.
    /// case ACTIVE
    ///     This case waits for either a button imput or for a gameOverFlag.
    ///     If the gameOverFlag is called the state will be put back to INACTIVE.
    ///     If a button is pressed it will be put into the buttonPool.
    ///     The pool will be read, if its the correct buttonID,
    ///     the sendControl will be sent the shot message needed.
    void main(){
        unsigned int weapon;
        unsigned int playerID;
        unsigned int button;
        timerWeaponCooldown.set(100);
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
                            hwlib::wait_ms(3);
                            sendControl.sendMessage(((playerID<<5)+ weapon)|0b011000 );
                            timerWeaponCooldown.set(cooldownForWeapon[weapon]*1'000);
                        }
                    }else if(event == gameOverFlag){
                        state = INACTIVE;
                    }
                    break;
            }
        }
    }
};

#endif // SHOT_CONTROL_HPP
