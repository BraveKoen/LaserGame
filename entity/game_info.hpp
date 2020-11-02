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
    rtos::pool<std::array<std::array<uint8_t, 2>,100>> hitsPool;

    unsigned int count = 0;

public:
    GameInfo():
        playerIDPool("playerIDPool"),
        gameTimePool("gameTimePool"),
        weaponTypePool("weaponTypePool"),
        hitsPool("hitsPool")
    {}

    void setWeapon(int type) { weaponTypePool.write(type); }
    
    int getWeapon() { return weaponTypePool.read(); }

    void setPlayerID(int id) { playerIDPool.write(id); }

    int getPlayerID() { return playerIDPool.read(); }

    void setTime(int time) { gameTimePool.write(time); }
    
    int getTime() { return gameTimePool.read(); }

    void registerHit(uint8_t playerID,uint8_t damage){
        std::array<std::array<uint8_t, 2>,100> tempHits= hitsPool.read();
        tempHits[count][0] = playerID;
        tempHits[count][1] = damage;
        hitsPool.write(tempHits);
        count++;
    }

    std::array<std::array<uint8_t, 2>,100> getHits(){
        return hitsPool.read();
    }

    void clearHits(){
        std::array<std::array<uint8_t, 2>,100> tempHits= hitsPool.read();
        for(unsigned int i = 0; i<=count; i++){
            tempHits[i][0] = 0;
            tempHits[i][1] = 0;
        }
        hitsPool.write(tempHits);
        count = 0;
    }

};

#endif //GAME_INFO_HPP