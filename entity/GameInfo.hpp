#include "hwlib.hpp"
#include "rtos.hpp"

class GameInfo : public rtos::task<> {
private:
    rtos::pool<int> playerID;
    rtos::pool<int> gameTime;
    rtos::pool<int> weaponType;

    unsigned int PlayerID;
    unsigned int gameTime;
    unsigned int weaponType;
    int hits[100][2];

public:
    GameInfo():
        task("GameInfo"),
        playerID("playerIDPool"),
        gameTime("gameTimePool"),
        weaponType("weaponTypePool")

    {}

    void setWeapon(int type) { weaponType.write(type); }
    
    int getWeapon() { return weaponType.read(); }

    void setPlayerID(int id) { playerID.write(id); }

    int getPlayerID() { return PlayerID.read(); }

    void setTime(int time) { gameTime.write(time); }
    
    int getTime() { return gameTime.read(); }
};