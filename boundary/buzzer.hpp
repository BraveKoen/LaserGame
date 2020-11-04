#ifndef BUZZER_HPP
#define BUZZER_HPP

#include "rtos.hpp"
#include "hwlib.hpp"

class Buzzer{

private:
    hwlib::target::pin_out buzzerPin;

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

    void hitSound(){
        for( int i = 1; i < 4; i++ ){
            beep( 200/i, 75'000 );
            hwlib::wait_us( 20'000 );
        }
    }
};


#endif // BUZZER_HPP