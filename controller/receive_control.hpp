#ifndef RECEIVECONTROL_HPP
#define RECEIVECONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "IrReceiver.hpp"

class ReceiveControl : public rtos::task<>{
    enum states {WAITING, RECEIVINGMESSAGE};

private:
    states state = WAITING;

    rtos::clock clock_200us;
    rtos::timer timer_4ms;

    IrReceiver irReceiver;
    Decoder decoder;

public:
    ReceiveControl():
    task("ReceiveTask"),
    clock_200us(this, 200, "200 us clock"), 
    timer_4ms(this, "4 ms timer"),
    irReceiver(hwlib::target::pins::d8),
    decoder()
    {}

private:
    void main(){
        bool signal=0;
        uint_fast8_t messageSize=0;
        uint_fast16_t message=0;
        uint_fast8_t signalHigh=0;

        for(;;){
            switch(state)
            {
                case WAITING:
                    wait(clock_200us);
                    signal = irReceiver.read();
                    if(signal){
                        messageSize=0;
                        message=0;
                        signalHigh=1;
                        timer_4ms.clear();
                        timer_4ms.set(4'000);
                        state = RECEIVINGMESSAGE;
                    }
                    break;
                case RECEIVINGMESSAGE:
                    auto event = wait(clock_200us + timer_4ms);
                    if(event == clock_200us){
                        signal = irReceiver.read();
                        if(signal){
                            signalHigh++;
                        }else if(signalHigh>0){
                            if(signalHigh<6){
                                message=message<<1;
                                messageSize++;
                            }else{
                                message=message<<1;
                                message++;
                                messageSize++;
                            }
                            if(messageSize<=16){
                                timer_4ms.set(4'000);
                                signalHigh=0;
                            }else{
                                //hwlib::cout << "Message: " << message << "\n"; //also comment out all decoder related things if that hasnt been made yet.
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
};

#endif