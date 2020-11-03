#include "hwlib.hpp"
#include "rtos.hpp"

#include "boundary/display.hpp"
#include "boundary/keypad.hpp"
#include "controller/decoder.hpp"
#include "controller/game_time_control.hpp"
#include "controller/init_control.hpp"
#include "controller/receive_control.hpp"
#include "controller/receive_hit_control.hpp"
#include "controller/register_control.hpp"
#include "controller/send_control.hpp"
#include "controller/shot_control.hpp"
#include "controller/transfer_control.hpp"
#include "entity/game_info.hpp"

extern unsigned char bmptk_stack[ 81920 ];
extern unsigned int __stack_start;
extern unsigned int __stack_end;

int main() {
    hwlib::wait_ms(1'000);

    auto gameInfo = GameInfo();
    auto sendControl = SendControl();
    auto transferControl = TransferControl(gameInfo);
    namespace target = hwlib::target;
    auto out0 = target::pin_oc(target::pins::a0);
    auto out1 = target::pin_oc(target::pins::a1);
    auto out2 = target::pin_oc(target::pins::a2);
    auto out3 = target::pin_oc(target::pins::a3);
    auto in0  = target::pin_in(target::pins::a4);
    auto in1  = target::pin_in(target::pins::a5);
    auto in2  = target::pin_in(target::pins::a6);
    auto in3  = target::pin_in(target::pins::a7);
    auto outPort = hwlib::port_oc_from(out0, out1, out2, out3);
    auto inPort  = hwlib::port_in_from(in0, in1, in2, in3);
    auto keypad = Keypad(outPort, inPort);
    auto shotControl = ShotControl(sendControl, gameInfo, keypad);
    auto display = Display(
        target::pins::scl,
        target::pins::sda
    );
    auto receiveHitControl = ReceiveHitControl(
        gameInfo,
        shotControl,
        display
    );
    auto gameTimeControl = GameTimeControl(
        gameInfo,
        shotControl,
        receiveHitControl,
        display
    );
    auto initControl = InitControl(
        gameInfo,
        sendControl,
        transferControl,
        gameTimeControl,
        keypad,
        display
    );
    auto registerControl = RegisterControl(
        gameInfo,
        display,
        keypad
    );

    auto receiveControl = ReceiveControl(
        receiveHitControl,
        gameTimeControl,
        registerControl,
        transferControl
    );
   //  (void)initControl;
   //  (void)registerControl;
    rtos::run();

    //
    //  auto sendTest = SendControl();
    //  sendTest.sendMessage(0b0'11111);
    //  sendTest.sendMessage(0b0);
    //  sendTest.sendMessage(0b0'01010);
    //  sendTest.sendMessage(0b0'00001);

    //  auto receive_control = ReceiveControl();
    //  rtos::run();
}
