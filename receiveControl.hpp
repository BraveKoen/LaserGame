#ifndef RECEIVECONTROL_HPP
#define RECEIVECONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

class ReceiveControl : public rtos::task<>{
    enum states = {WAITING, RECEIVINGMESSAGE};

private:
    states state = WAITING;

    rtos::clock clock_400us;
    rtos::timer timer_4ms;

    IrReceiver irReceiver;
    Decoder decoder;

    void main(){
        bool signal=0;
        uint_fast8_t messageSize=0;
        uint_fast16_t message=0;
        uint_fast8_t signalHigh=0;
        uint_fast8_t signalLow=0;

        for(;;){
            switch(state)
            {
                case WAITING:
                    wait(clock_400us);
                    signal = irReceiver.read();
                    if(signal){
                        messageSize=0;
                        message=0;
                        signalHigh=1;
                        signalLow=0;
                        timer_4ms.set();
                        state = RECEIVINGMESSAGE;
                    }
                    break;
                case RECEIVINGMESSAGE:
                    auto event = wait(clock_400us + timer_4ms);
                    if(event == clock_400us){
                        signal = irReceiver.read();
                        if(signal && signalHigh<4){
                            signalHigh++;
                        }else if(!signal && signalLow<4){
                            signalLow++;
                        }else{
                            signalHigh+=signal;
                            signalLow+=(1-signal);
                            if(signalHigh==2 && signalLow==4){
                                message<<1;
                                messageSize++;
                            }else if(signalHigh==4 && signalLow==2){
                                message<<1;
                                message++;
                                messageSize++;
                            }else{
                                break;
                            }
                            if(messageSize<=16){
                                timer_4ms.set();
                                signalHigh=0;
                                signalLow=0;
                            }else{
                                decoder.decode(message);
                                state = WAITING;
                            }
                        }
                    }else if(event == timer_4ms){
                        state = WAITING;
                    }
                    break;
            }
        }
    }

public:
    ReceiveControl():
    task("ReceiveControl"),
    clock_400us(this, 400, "400 us clock"),
    timer_4ms(this, 4'000, "4 ms timer"),
    irReceiver(),
    decoder()
};

#endif