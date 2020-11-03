#ifndef TRANSFER_CONTROL_HPP
#define TRANSFER_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../entity/game_info.hpp"
#include <array>

    /// \brief
    /// This class TransferControl puts the hits received into the pc terminal. 
    /// \details
    /// When certain flags are set this function will hwlib::cout the hits from ../entity/game_info.
class TransferControl : public rtos::task<>{
enum state_t { INACTIVE, ACTIVE };
private:
    GameInfo& gameInfo;
    rtos::flag transferFlag;
    state_t state = INACTIVE;
public:
    /// \brief
    /// Contructor for class TransferControl
    /// \details
    /// This constructor gives TransferControl a default task since only 1 instance is needed. 
    /// It receives the gameinfo by reference.
    /// It also has a flag called transferFlag.
    TransferControl(GameInfo& gameInfo):
    task("TransferControl"),
    gameInfo(gameInfo),
    transferFlag(this, "transferFlag"){}

    /// \brief
    /// Public Function transferCommand. Sets Flag.
    /// \details
    /// This function simply sets the transferFlag when called.
    void transferCommand(){
        transferFlag.set();
    }

private:
    /// \brief
    /// This private function puts the hits received in a hwlib::cout if requested to do so.
    /// \details
    /// case INACTIVE
    ///     When this case is at play, it waits until the transferflag is set.
    ///     Then it puts the state to active and breaks so it can go back to the switch and move to the active state.
    /// case ACTIVE
    ///     This case pulls the hits from the gameInfo and hwlib::cout's them into the terminal of the connected PC,
    ///     (wiht an open terminal able to read).
    ///     After completing its task it will put the state back to inactive.
    void main(){
        for(;;){
            switch(state){
                case INACTIVE:
                    wait(transferFlag);
                    state = ACTIVE;
                    break;
                case ACTIVE:
                    auto array_hits = gameInfo.getHits();
                    hwlib::cout << "Player data: "<< gameInfo.getPlayerID() << "\n";
                    for(auto hit : array_hits){
                        if(hit[1] == 0){
                            break;
                        }
                        hwlib::cout << "Player: " << hit[0] << " Damage done: "<< hit[1]<< "\n";
                    }
                    state = INACTIVE;
                    break;
            }
        }
    }

};

#endif // TRANSFER_CONTROL_HPP
