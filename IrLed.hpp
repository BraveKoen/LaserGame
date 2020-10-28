class IrLed {
 private:
  hwlib::target::d2_36kHz irLed;

 public:
  IrLed() : irLed(hwlib::target::d2_36kHz()) {}

  void write(int state) {
    irLed.write(state);
    irLed.flush();
  }
};