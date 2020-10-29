#ifndef RECEIVE_HIT_CONTROL_HPP
#define RECEIVE_HIT_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

class ReceiveHitControl : public rtos::task<>{

enum state_t {INACTIVE, ACTIVE};

private:
public:
    ReceiveHitControl(){
        task("ReceiveHitControl")
    }
private:
};


#endif // RECEIVE_HIT_CONTROL_HPP