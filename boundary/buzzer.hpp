#ifndef BUZZER_HPP
#define BUZZER_HPP

class ReceiveHitControl : public rtos::task<>{

enum state_t {INACTIVE, ACTIVE};

private:
    state_t state = INACTIVE;

    rtos::flag playSoundFlag;
    rtos::pool<bool> playSoundPool;

    hwlib::target::pin_out buzzer;


public:
    ReceiveHitControl(hwlib::target::pins pin):
        task("Buzzer"),
        buzzer(hwlib::target::pin_out(pin))
    {}

    void playSound(bool on){
        playSoundPool.write(on);
        playSoundFlag.set();
    }

private:
    void main(){
        bool state;
        for(;;){
            switch(state){
                case INACTIVE:
                    wait(playSoundFlag);
                    state = playSoundPool.read();
                    if(state){
                        state = ACTIVE;
                        break;
                    }else{
                        break;
                    }
                case ACTIVE:
                    //TIMER TIMEEEE

        }
    }
};


#endif // BUZZER_HPP