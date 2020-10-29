#ifndef RED_LED_HPP
#define RED_LED_HPP

class RedLed{
private:
    hwlib::target::pin_out redled;

public:
    RedLed(hwlib::target::pins pin):
    redled(hwlib::target::pin_out( pin)){}

void write(bool state){
    redled.write(state);
}
};

#endif