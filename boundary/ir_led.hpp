#ifndef IR_LED_HPP
#define IR_LED_HPP

/// \brief
/// Class IrLed
/// \details
/// IrLed that goes brrr
class IrLed {
 private:
  hwlib::target::d2_36kHz irLed;

 public:
  IrLed() : irLed(hwlib::target::d2_36kHz()) {}
/// \brief
/// Function write int state: void
/// \details
/// Writes state to the irled and flush it
  void write(int state) {
    irLed.write(state);
    irLed.flush();
  }
};
#endif // IR_LED_HPP
