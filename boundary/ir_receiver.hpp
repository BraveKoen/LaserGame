#ifndef IRRECEIVER_HPP
#define IRRECEIVER_HPP

class IrReceiver{
private:
    hwlib::target::pin_in irReceiver;

public:
    IrReceiver(hwlib::target::pins pin):
    irReceiver(hwlib::target::pin_in( pin)){}

    bool read(){
        irReceiver.refresh();
        return !irReceiver.read();
    }
};

#endif