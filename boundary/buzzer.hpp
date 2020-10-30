#ifndef BUZZER_HPP
#define BUZZER_HPP

class ReceiveHitControl : public rtos::task<>{

enum state_t {INACTIVE, ACTIVE};

private:
    state_t state = INACTIVE;

    rtos::clock clock_1500us;
    rtos::flag playSoundFlag;
    rtos::pool<bool> playSoundPool;

    hwlib::target::pin_out buzzer;


public:
    ReceiveHitControl(hwlib::target::pins pin):
        task("Buzzer"),
        clock_1500us(this, 1500, "1500 us clock"),
        buzzer(hwlib::target::pin_out(pin))
    {}

    void playSound(bool on){
        playSoundPool.write(on);
        playSoundFlag.set();
    }
    
private:
    void main(){
        bool poolState;
        bool buzzerState;
        for(;;){
            switch(state){
                case INACTIVE:
                    wait(playSoundFlag);
                    poolState = playSoundPool.read();
                    if(poolState){
                        buzzerState = true;
                        state = ACTIVE;
                        break;
                    }else{
                        break;
                    }
                case ACTIVE:
                    wait(clock_1500us);
                    if(buzzerState == true){
                        buzzerState = false;
                        buzzer.write(true);
                    }else{
                        buzzerState = true;
                        buzzer.write(false);
                    }
                    break;

        }
    }
};


#endif // BUZZER_HPP