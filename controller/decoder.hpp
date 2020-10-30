#ifndef DECODER_HPP
#define DECODER_HPP

#include "hwlib.hpp"
#include "rtos.hpp"

class Decoder : public rtos::task<>{

    enum states {INACTIVE, DECODING};
    enum subStates {DEFAULT, WAITINGFORTIME, WAITINGFORCOUNTDOWN};

private:
    states state = INACTIVE;
    subStates subState = DEFAULT;

    std::array<uint_fast8_t, 2> damageForType = {10, 25};

    rtos::channel<uint_fast16_t, 10> dataInChannel;

    //ReceiveHitControl & receiveHitControl;
    //GameTimeControl & gameTimeControl;
    //RegisterControl & registerControl;
    //TransferControl & transferControl;

public:
    Decoder(/*ReceiveHitControl & receiveHitControl, GameTimeControl & gameTimeControl, RegisterControl & registerControl, TransferControl & transferControl*/):
    task("DecoderTask"),
    dataInChannel(this, "dataInChannel")/*,
    receiveHitControl(receiveHitControl),
    gameTimeControl(gameTimeControl),
    registerControl(registerControl),
    transferControl(transferControl)*/
    {}

    void decodeData(const uint_fast16_t & data){
        dataInChannel.write(data);
    }

private:
    bool checkSum(uint_fast16_t data){ //Checksum. bits 1-5 XOR bits 6-10 == bits 11-15
        return (((data>>10) & 0b11111) ^ ((data>>5) & 0b11111))==(data & 0b11111);
    }

    void main(){
        uint_fast16_t data=0xffff; //16 1's. Data should never take this value aside from when initialized here.
        uint_fast16_t previousData=0;
        for(;;){
            switch(state){
                case INACTIVE:
                    previousData = data;
                    data = dataInChannel.read();
                    hwlib::wait_ms(1000); //Uncomment this in if testing with cout, gives ReceiveControl enough time to receive messages sent right after eachother, lengthen if need be.
                    hwlib::cout<<"INACTIVE: "<<hwlib::bin<<data<<hwlib::dec<<"\n";
                    state = DECODING;
                    break;
                case DECODING:
                    switch(subState){
                        case DEFAULT:
                            if(data==previousData){ //All data is sent twice, to increase succes rate. We dont want to do everything twice so we make sure to check first. 
                                previousData=0xffff;//The message can/should never be 0xffff so we can use that to make sure this is never true more than twice in a row.
                                hwlib::cout<<"DOUBLE: "<<hwlib::bin<<data<<hwlib::dec<<"\n";
                                state = INACTIVE;   //If someone is shot by the same player twice things should still mostly work even if one of the two messages is lost somewhere, it will simply continue on the second of the two.
                            }else{
                                if(checkSum(data)){ 
                                    if(data==0b0){ //Start command
                                        previousData=data;
                                        subState = WAITINGFORTIME;
                                        hwlib::cout<<"StartCommand\n";
                                        break;
                                    }else if(data==10000'10000){ //Check for transfer command
                                        //transferControl.transferCommand();
                                        hwlib::cout<<"Transfercomman\n";
                                        state = INACTIVE;
                                    }else if(((data>>8) & 0b11)==0b11){ //Check for shot command.
                                        //receiveHitControl.hitReceived(((data>>10) & 0b11111), damageForType[(data>>5) & 0b111)]);
                                        hwlib::cout<<"Hitreceived, PlayerID: "<<((data>>10) & 0b11111)<<" Damage: "<<damageForType[(data>>5) & 0b111)]<<"\n";
                                    } //More commands can be added here, would be in format 0b0'ppppp'10ccc'xxxxx. Where p is player number, c is command(number) and x is the XOR of ppppp and 10ccc
                                }
                            }
                            state = INACTIVE;
                            break;
                        case WAITINGFORTIME:
                            data = dataInChannel.read();
                            //hwlib::cout<<hwlib::bin<<data<<hwlib::dec;
                            if(data==previousData){ 
                                previousData=0xffff;
                                hwlib::cout<<"DOUBLE: "<<hwlib::bin<<data<<hwlib::dec<<"\n";
                            }else{
                                if(checkSum(data)){
                                    if((!((data>>9)&0b01)) && (!((data>>10)&0b011111))){ //Check for time message
                                        //registerControl.gameTime((data>>5) & 0b1111);
                                        hwlib::cout<<"Time: "<<((data>>5) & 0b1111)<<"\n";
                                        previousData=data;
                                        subState = WAITINGFORCOUNTDOWN;
                                    }else{
                                        subState = DEFAULT;
                                    }
                                }
                            }
                            break;
                        case WAITINGFORCOUNTDOWN:
                            data = dataInChannel.read();
                            //hwlib::cout<<hwlib::bin<<data<<"\n";
                            if(data==previousData){ 
                                previousData=0xffff;
                                hwlib::cout<<"DOUBLE: "<<hwlib::bin<<data<<hwlib::dec<<"\n";
                            }else{
                                if(checkSum(data)){
                                    if((!((data>>9)&0b01)) && (!((data>>10)&0b011111))){ //Check for time message
                                        //gameTimeControl.start((data>>5) & 0b1111));
                                        hwlib::cout<<"CD: "<<((data>>5) & 0b1111)<<"\n";
                                        previousData=data;
                                        subState = DEFAULT;
                                        state = INACTIVE;
                                    }else{
                                        subState = DEFAULT;
                                    }
                                }
                            }
                            break;
                    }
            }
        }
    }
};

#endif //DECODER_HPP