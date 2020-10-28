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
    //Decoder decoder;

public:
    ReceiveControl():
    task("ReceiveTask"),
    clock_200us(this, 200, "200 us clock"), 
    timer_4ms(this, "4 ms timer"),
    irReceiver(hwlib::target::pins::d8)//,
    //decoder()
    {}

private:
    void main(){
        bool signal=0;
        uint_fast8_t messageSize=0;
        uint_fast16_t message=0;
        uint_fast8_t signalHigh=0;
        //int debug=0;

        for(;;){
            //hwlib::cout<<"ReceiveControl main loop\n";
            switch(state)
            {
                case WAITING:
                    //hwlib::cout<<"ReceiveControl WAITNG\n";
                    wait(clock_200us);
                    signal = irReceiver.read();
                    //hwlib::cout<<signal<<" signal\n";
                    if(signal){
                        messageSize=0;
                        message=0;
                        signalHigh=1;
                        timer_4ms.clear();
                        timer_4ms.set(4'000);
                        //debug=1;
                        state = RECEIVINGMESSAGE;
                    }
                    break;
                case RECEIVINGMESSAGE:
                    //hwlib::cout<<"ReceiveControl RECEIVINGMESSAGE\n";
                    auto event = wait(clock_200us + timer_4ms);
                    if(event == clock_200us){
                        signal = irReceiver.read();
                        if(signal){
                            signalHigh++;
                            //debug=debug<<1;
                            //debug++;
                        }else if(signalHigh>0){
                            if(signalHigh<6){
                                message=message<<1;
                                messageSize++;
                                //hwlib::cout<<"message: "<<message<<" message size: "<<messageSize<<"\n";
                            }else{
                                message=message<<1;
                                message++;
                                messageSize++;
                            }
                            //debug=debug<<1;
                            if(messageSize<=16){
                                //hwlib::cout<<"YEET";
                                timer_4ms.set(4'000);
                                signalHigh=0;
                                //debug=0;
                            }else{
                                //decoder.decode(message);
                                hwlib::cout<<"message: ";
                                hwlib::cout <<hwlib::dec<< message << "\n";
                                state = WAITING;
                            }
                        }
                        //hwlib::cout<<"high: "<<signalHigh<<" low: "<<signalLow<<" debug: "<<debug<<"\n";
                        
                        
                    }else if(event == timer_4ms){
                        hwlib::cout<<"TIMEOUT, message length: "<<hwlib::dec<<messageSize<<"\n";
                        state = WAITING;
                    }
                    break;
            }
        }
    }


};

#endif