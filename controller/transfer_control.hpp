#ifndef TRANSFER_CONTROL_HPP
#define TRANSFER_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../entity/game_info.hpp"
#include <array>

class TransferControl : public rtos::task<>{
enum state_t { INACTIVE, ACTIVE };
private:
    GameInfo& gameInfo;
    rtos::flag transferFlag;
    state_t state = INACTIVE;
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
                    for(auto hit : array_hits){
                        hwlib::cout << "Player: " << hit[0] << " Damage done: "<< hit[1]<< "\n";
                        if(hit[1] == 0){
                            break;
                        }
                    }
                    state = INACTIVE;
                    break;
            }
        }
    }

};

#endif // TRANSFER_CONTROL_HPP
