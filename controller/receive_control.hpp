#ifndef RECEIVECONTROL_HPP
#define RECEIVECONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../boundary/ir_receiver.hpp"
#include "decoder.hpp"

/// \brief
/// Class ReceiveControl.
/// \details
/// This class is for timings
class ReceiveControl : public rtos::task<>{
    enum states {WAITING, RECEIVINGMESSAGE};

private:
    states state = WAITING;

    rtos::clock clock_200us;
    rtos::timer timer_4ms;

    IrReceiver irReceiver;
    Decoder decoder;

public:
    /// \brief
    /// Constructor ReceiveControl
    /// \details
    /// This constructor has receiveHitControl, gameTimeControl, registerControl and transferControl by reference.
    /// IrReceiver is created with pin d8.
    /// clock_200us, timer_4ms are created.
    ReceiveControl(ReceiveHitControl & receiveHitControl, GameTimeControl & gameTimeControl, RegisterControl & registerControl, TransferControl & transferControl):
    task("ReceiveTask"),
    clock_200us(this, 200, "200 us clock"), 
    timer_4ms(this, "4 ms timer"),
    irReceiver(hwlib::target::pins::d8),
    decoder(receiveHitControl, gameTimeControl, registerControl, transferControl)
    {}

private:
    /// \brief
    /// main ReceiveControl
    /// \details
    /// states are WAITING and RECEIVINGMESSAGE
    /// case WATING
    ///     Waits for 200 us then reads IrReceiver.
    ///     if there is a signal it will change the state to RECEIVING and signalhigh = 1.
    /// case RECEIVINGMESSAGE
    ///     Will check every 200us for a new signal
    void main(){
        bool signal=0;
        uint_fast8_t messageSize=0;
        uint_fast16_t message=0;
        uint_fast8_t signalHigh=0;
        for(;;){
            switch(state){
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
                                //hwlib::cout << hwlib::bin << "Message: " << message << "\n" << hwlib::dec; //also comment out all decoder related things if that hasnt been made yet.
                                decoder.decodeData(message);
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

#endif // RECEIVE_CONTROL_HPP