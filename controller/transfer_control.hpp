#ifndef TRANSFER_CONTROL_HPP
#define TRANSFER_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

class TransferControl : public rtos::task<>{
enum state_t { INACTIVE, ACTIVE };
private:
    state_t state = INACTIVE;

    rtos::flag transferFlag;

    GameInfo& gameInfo;

public:
    TransferControl(GameInfo& gameInfo):
    task("TransferControl"),
    gameInfo(gameInfo),
    transferFlag(this, "transferFlag"){}

    void transferCommand(){
        transferFlag.set();
    }

private:
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
                    for(int i : array_hits){
                        hwlib::cout << "Player: " << array_hits[0] << " Damage done: "<< array_hits[1]<< "\n";
                        if(array_hits[1] == 0){
                            break;
                        }
                    }
                    state = INACTIVE;
            }
        }
    }

};

