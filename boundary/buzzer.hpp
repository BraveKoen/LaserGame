#ifndef BUZZER_HPP
#define BUZZER_HPP

#include "rtos.hpp"
#include "hwlib.hpp"

class Buzzer : public rtos::task<>{

enum state_t {INACTIVE, ACTIVE};

private:
    hwlib::target::pin_out buzzerPin;
    rtos::clock clock_1500us;
    rtos::pool<bool> playSoundPool;
    rtos::flag playSoundFlag;

    state_t state = INACTIVE;
public:
    Buzzer(hwlib::target::pins pin):
        task("Buzzer"),
        buzzerPin(hwlib::target::pin_out(pin)),
        clock_1500us(this, 1500, "1500 us clock"),
        playSoundPool("playSoundPool"),
        playSoundFlag(this, "paySoundFlag")
    {}

    void playSound(bool on){
        playSoundPool.write(on);
        playSoundFlag.set();
    }


    void await( long long unsigned int t ){
        while( t > hwlib::now_us() ){}
    }

    void beep(int f, int d, int fd = 1000 ){
        auto t = hwlib::now_us();    
        auto end = t + d;
        while( end > hwlib::now_us() ){
            auto p = 500'000 / f;
            f = f * fd / 1000;
            buzzerPin.write( 1 );
            buzzerPin.flush();
            await( t += p );
            buzzerPin.write( 0 );
            buzzerPin.flush();
            await( t += p );
        }   
    }

    void peew(){
        beep(20'000, 200'000, 990 );
    }
    
private:
    void main(){
        bool poolState;
        bool buzzerState;
        for(;;){
            switch(state){
                case INACTIVE:
                    wait(playSoundFlag);
                    poolState = playSoundPool.read();
                    if(poolState){
                        buzzerState = true;
                        state = ACTIVE;
                        break;
                    }else{
                        break;
                    }
                case ACTIVE:
                    auto event = wait(clock_1500us + playSoundFlag);
                    if(event == clock_1500us){
                        if(buzzerState == true){
                            buzzerState = false;
                            buzzerPin.write(true);
                        }else{
                            buzzerState = true;
                            buzzerPin.write(false);
                        }
                    }else if(event == playSoundFlag){
                        poolState = playSoundPool.read();
                        if(!poolState){
                            buzzerPin.write(false);
                            state = INACTIVE;
                        }
                        break
                    }
                    break;
            }
        }
    }
};


#endif // BUZZER_HPP