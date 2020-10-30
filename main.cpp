#include "hwlib.hpp"
#include "rtos.hpp"
#include "controller/send_control.hpp"
#include "controller/receive_control.hpp"
#include "boundary/ir_receiver.hpp"
#include "controller/decoder.hpp"
#include "controller/shot_control.hpp"

extern unsigned char bmptk_stack[ 81920 ];

extern unsigned int __stack_start;
extern unsigned int __stack_end;


int main( void ){	  
   
   hwlib::wait_ms(1000);

   auto sendTest = SendControl();
   sendTest.sendMessage(0b0'11111);
   sendTest.sendMessage(0b0);
   sendTest.sendMessage(0b0'01010);
   sendTest.sendMessage(0b0'00001);

   auto receive_control = ReceiveControl();
   rtos::run();
}
