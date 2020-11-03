#ifndef SEND_CONTROL_HPP
#define SEND_CONTROL_HPP

#include "hwlib.hpp"
#include "rtos.hpp"
#include "../boundary/red_led.hpp"
#include "../boundary/ir_led.hpp"

/// \brief
/// Class SendControl lets other classes shot messages via a Red and IR led.
/// \details
/// This class shoots a message via the IrLed and RedLed
class SendControl : public rtos::task<>{

enum state_t {INACTIVE, SENDING};

private:
    state_t state = INACTIVE;

    rtos::channel<uint_fast16_t, 20> messageChannel;
    RedLed redLed;
    IrLed irLed;
public:
    /// \brief
    /// This contstructor initializes the calls SendControl with a fixed task.
    /// \details
    /// This Constructor doesnt need any info to do it job, it make a RedLed in pin d7 and a IRLed(default D2).
    /// It also initializes a messageChannel.
    SendControl():
    task( "SendControl" ),
    messageChannel( this, "messageChannel" ),
    redLed( hwlib::target::pins::d7 ),
    irLed()
    {}

    /// \brief
    /// This public function lets other functions put a message into the messageChannel.
    /// \details
    /// The function puts a uint_fast16_t into a messageChannel.
    /// To make it so that every message is sent twice we put it into the channel twice.
    void sendMessage(uint_fast16_t message){
        messageChannel.write(message);
        messageChannel.write(message);
    }

private:
    /// \brief
    /// This private function main will wait for a message, fix it and send it directly.
    /// \details
    /// case INACTIVE
    ///     When inactive it waits for a message in the messageChannel.
    ///     It will then shift this message and add the XOR bits at the end.
    ///     After putting the counter to 0 it will go to state sending
    /// case SENDING
    ///     This state will mask over the message one by one and send it bit by bit.
    ///     When starting the process it will turn on the red led,
    ///     When done with all bits it will turn off the red led.
    ///     The ir led has to be turned on and off with specific timings,
    ///     To write a 1 we turn on the led for 1600 us, then off for 800.
    ///     To write a 0 we turn on the led for 800 us, then off for 1600.
    void main(){
        uint_fast16_t message;
        unsigned int counter;

        bool bit;
        for(;;){
            switch(state){
                case INACTIVE:
                    message = messageChannel.read();
                    message = message<<5;
                    message += ((message>>10) & 0b11111) ^ ((message>>5) & 0b11111);
                    counter = 0;
                    state = SENDING;
                    break;

                case SENDING:
                    bit = message & (0b1<<16);
                    message = message<<1;
                    if(counter == 0)redLed.write(1);
                    if(bit == 1){
                        irLed.write(1);
                        hwlib::wait_us(1600);
                        irLed.write(0);
                        hwlib::wait_us(800);
                    }else{
                        irLed.write(1);
                        hwlib::wait_us(800);
                        irLed.write(0);
                        hwlib::wait_us(1600);
                    }
                    if(counter < 16){
                        counter++;
                    }else{
                        hwlib::wait_ms(3);
                        state = INACTIVE;
                        redLed.write(0);
                    }
                    break;
            }
        }
    }
};

#endif // SEND_CONTROL_HPP