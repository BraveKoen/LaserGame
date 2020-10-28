#include "hwlib.hpp"
#include "rtos.hpp"
#include "SendControl.hpp"



extern unsigned char bmptk_stack[ 81920 ];

extern unsigned int __stack_start;
extern unsigned int __stack_end;


int main( void ){	  
   
   hwlib::wait_ms(1000);

   auto sendTest = SendControl("sendTest");
   sendTest.sendMessage(0b1111111111111111);
   rtos::run();
}