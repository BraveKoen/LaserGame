#include "hwlib.hpp"
#include "rtos.hpp"

class sendControl : public rtos::task<>{
    enum state_s       = {INACTIVE, SENDING};

private:
    auto ir = hwlib::target::d2_36kHz();
    state_s state = INACTIVE;

    rtos::channel messageChannel;
    RedLed redLed;
    IrLed irLed;
public:
    sendControl(RedLed redLed):
    redLed(redLed)
    {}

    void sendMesasge(uint_fast16_t message){
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
                
                auto evt = wait(messageChannel);
                if(evt=messageChannel){
                    message = messageChannel.read(); 
                    counter = 0;

                    state = SENDING;
                }
                break;

                case SENDING:
                bit = message & (0b1<<16);
                message = message<<1;
                if(bit == 1){
                    redLed.write(1);
                    irLed.write(1);
                    hwlib::wait_us(1600);
                    irLed.write(0);
                    hwlib::wait_us(800);
                    redLed.write(0);
                }else{
                    redLed.write(1);
                    irLed.write(1);
                    hwlib::wait_us(800);
                    irLed.write(0);
                    hwlib::wait_us(1600);
                    redLed.write(0);
                }
                if(counter < 16){
                    counter++;
                }else{
                    state = INACTIVE;
                }
                break;
                default: break;
            }
        }
    }

};

class IrLed{
private:
    auto ir = hwlib::target::d2_36kHz();

public:
void write(int state){
    ir.write(state);
    ir.flush();
}
};

class RedLed{
private:
    hwlib::pin_out & led;

public:
    RedLed(hwlib::pin_out &led):
    led(led){}

void write(bool state){
    led.write(state);
    led.flush();
}
};



