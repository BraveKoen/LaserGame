#ifndef BUZZER_HPP
#define BUZZER_HPP

#include "rtos.hpp"
#include "hwlib.hpp"

class Buzzer{

private:
    hwlib::target::pin_out buzzerPin;

    state_t state = INACTIVE;
public:
    Buzzer(hwlib::target::pins pin):
        buzzerPin(hwlib::target::pin_out(pin))
    {}


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
};


#endif // BUZZER_HPP