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

    //unsigned int PlayerID;
    //unsigned int gameTime;
    //unsigned int weaponType;
    //int hits[100][2];
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
};

#endif