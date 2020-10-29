class GameInfo{
private:
    unsigned int PlayerID;
    unsigned int gameTime;
    unsigned int weaponType;
    int hits[100][2];

public:
   GameInfo():{}

   void setWeapon(int type){
       weaponType = type;
   }
   int getWeapon(){
       return weaponType;
    }    
   
    void setPlayerID(int id){
       PlayerID = id;
    }//stupid

    int getPlayerID(){
        return PlayerID;
    }

   void setTime(int time){
       gameTime = time;
    }
   int getTime(){
       return gameTime;
    }

};