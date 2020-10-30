#ifndef GAME_INFO_HPP
#define GAME_INFO_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include <array>

class GameInfo{
private:
    rtos::pool<int> playerIDPool;
    rtos::pool<int> gameTimePool;
    rtos::pool<int> weaponTypePool;
    rtos::pool<std::array<std::array<uint8_t, 2>,100>> hits;

    unsigned int count = 0;

public:
    GameInfo():
        playerIDPool("playerIDPool"),
        gameTimePool("gameTimePool"),
        weaponTypePool("weaponTypePool")

    {}

    void setWeapon(int type) { weaponType.write(type); }
    
    int getWeapon() { return weaponType.read(); }

    void setPlayerID(int id) { playerID.write(id); }

    int getPlayerID() { return PlayerID.read(); }

    void setTime(int time) { gameTime.write(time); }
    
    int getTime() { return gameTime.read(); }

    void registerHits(uint8_t playerID,uint8_t damage){
        hits[count][0] = playerID;
        hits[count][1] = damage;
        count++;
    }

    std::array<std::array<uint8_t, 2>,100> getHits(){
        return hits.read();
    }

    void clearHits(){
        for(unsigned int i = 0; i<=count; i++){
            hits[i][0] = 0;
            hits[i][1] = 0;
        }
        count = 0;
    }

};

#endif //GAME_INFO_HPP