#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2016
#
# This file is in the public domain.
# 
#############################################################################

# source files in this project (main.cpp is automatically assumed)
SOURCES := init_control.cpp receive_hit_control.cpp game_time_control.cpp register_control.cpp

# header files in this project
HEADERS := button_listener.hpp button.hpp send_control.hpp ir_led.hpp red_led.hpp init_control.hpp register_control.hpp shot_control.hpp receive_control.hpp ir_receiver.hpp decoder.hpp game_time_control.hpp receive_hit_control.hpp transfer_control.hpp buzzer.hpp display.hpp glcd_oled_cooperative.hpp keypad.hpp trigger.hpp game_info.hpp

# other places to look for files for this project
SEARCH  := controller boundary entity interface

# set RELATIVE to the next higher directory and defer to the Makefile.due
RELATIVE := $(RELATIVE)../
include $(RELATIVE)Makefile.due
