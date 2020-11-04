#ifndef IRRECEIVER_HPP
#define IRRECEIVER_HPP

/// \brief
/// Class IrReceiver
/// \details
/// IrReceiver receives data from ir light
class IrReceiver{
private:
    hwlib::target::pin_in irReceiver;

public:
/// \brief
/// Constructor IrReceiver needs a hwlib::target::pin
    IrReceiver(hwlib::target::pins pin):
    irReceiver(hwlib::target::pin_in( pin)){}

/// \brief
/// Function read: void
/// \details
/// reads for a high pin
    bool read(){
        irReceiver.refresh();
        return !irReceiver.read();
    }
};

#endif // IRRECEIVER_HPP