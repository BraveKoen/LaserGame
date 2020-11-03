#ifndef RED_LED_HPP
#define RED_LED_HPP
/// \brief
/// Class REDLED
/// \details
/// REDLED is a indication that you are sending data
class RedLed{
private:
    hwlib::target::pin_out redled;

public:
/// \brief
/// Constuctor REDLED
/// \details
/// RedLed needs a hwlib::target::pin

    RedLed(hwlib::target::pins pin):
    redled(hwlib::target::pin_out( pin))
    {}
/// \brief
/// Function write(bool state): void
/// \details
/// Write the given state to the redled pin.
    void write(bool state){
        redled.write(state);
    }
};

#endif // RED_LED_HPP
