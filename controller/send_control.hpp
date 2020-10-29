#include "hwlib.hpp"
#include "rtos.hpp"
#include "../boundary/red_led.hpp"
#include "../boundary/ir_led.hpp"


class SendControl : public rtos::task<>{

enum state_t {INACTIVE, SENDING};

private:
    state_t state = INACTIVE;

    rtos::channel<uint_fast16_t, 10> messageChannel;
    RedLed redLed;
    IrLed irLed;
public:
    SendControl():
    task( "SendControl" ),
    messageChannel( this, "messageChannel" ),
    redLed( hwlib::target::pins::d7 ),
    irLed()
    {}

    void sendMessage(uint_fast16_t message){
        messageChannel.write(message);
    }

private:
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
                    if(counter < 15){
                        counter++;
                    }else{
                        state = INACTIVE;
                        redLed.write(0);
                    }
                    break;
            }
        }
    }
};