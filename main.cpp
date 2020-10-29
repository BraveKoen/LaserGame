#include "hwlib.hpp"
#include "rtos.hpp"
#include "controller/send_control.hpp"
#include "controller/receive_control.hpp"

extern unsigned char bmptk_stack[ 81920 ];

extern unsigned int __stack_start;
extern unsigned int __stack_end;


int main( void ){	  
   
   hwlib::wait_ms(1000);

   //auto sendTest = SendControl();
   auto receiveTest = ReceiveControl();
   //sendTest.sendMessage(0b0000011111);
   rtos::run();
}
