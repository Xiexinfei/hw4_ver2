#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

using namespace std::chrono;

DigitalInOut ping(D10);
Timer T;

BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
DigitalIn encoder(D11);
volatile int steps;
volatile int last;

BBCar car(pin5, pin6, servo_ticker);

char s;
int i = 0;
int j = 0, D = 0;
int BACK = 0;
int dot = 0, dot_ = 0;
int digit =0, digit_ =0;
char recv[1];
double L, ang; 
// L: distance between car and Apriltag(horizontal)
// ang: angle from Apriltag to car

Thread t;
Thread B;

int NUM(char);


void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;

}

void cargo(){
    while(1){
        printf("L = %f, ang = %f\n", L, ang);
        ThisThread::sleep_for(2s);

        if(ang != 0){
            ThisThread::sleep_for(10ms);
            printf("OK, L=%f, ang = %f\n", L, ang);

        for(j = 0; j < 6; j++){
        // turn right
            car.turn(200, -0.3);
            ThisThread::sleep_for(350ms);
        }
        car.stop();
        ThisThread::sleep_for(1s);

        int M = int(L);
        //printf("M=%d\n", M);

        for(j = 0; j < (int(150*M/23)); j++){  // 23cm
        // move to the AprilTag center
            car.goStraight(-100);
            ThisThread::sleep_for(20ms);
        }
        printf("end back\n");

        ThisThread::sleep_for(1s);
        car.stop();

        for(j = 0; j < 6; j++){
        // turn left
            car.turn(180, 0.3);
            ThisThread::sleep_for(180ms);
        }


        car.stop();
        ThisThread::sleep_for(3s);


/////////////////////////////PING//////////////////////////

        float val;

        ping.output();
        ping = 0; wait_us(200);
        ping = 1; wait_us(5);
        ping = 0; wait_us(5);

        ping.input();
        while(ping.read() == 0);
        T.start();
        while(ping.read() == 1);
        val = T.read();
        printf("Ping = %lf\r\n", val*17700.4f);
        T.stop();
        T.reset();

        ThisThread::sleep_for(1s);
      ///////////////////////////////
        return;
        }
    }
}


int NUM(char c){    // check if value from openMV number
    if (c == '0' || c == '1'|| c == '2'|| c == '3'
    || c == '4'|| c == '5'|| c == '6'|| c == '7'
    || c == '8'|| c == '9'|| c == '.') return 1;
    else return 0;
}


int main(){
    pc.set_baud(9600);
    encoder_ticker.attach(&encoder_control, 10ms);
    steps = 0;
    last = 0;

    t.start(cargo);




   uart.set_baud(9600);
   while(1){
      if(uart.readable()){
            uart.read(recv, sizeof(recv));
            
            s = recv[0];

            if(s == 'L'){
                i++;
            }else if (s == 'y'){
                i++;
            }else if (s == '='){
                i++;
            }else if (s == 'R'){
                i++;
            }else if (s == ','){
                i++;
            }
            
            ///////store for L///////
            if(i == 2 && NUM(recv[0]) == 1){
                if(s == '.'){
                    dot = 1;
                    digit = 1;
                }
                else if(s != '.' && dot != 1){
                    L = L * 10 +((int)s) - 48;
                }else if(s != '.' && dot == 1){
                    L = L + (pow(0.1,digit))*(((int)s) - 48);
                    digit++;
                }
            }
            /////////////////////////
            ////////store for Ry/////
            else if (i == 6 && NUM(recv[0]) == 1){
                if(s == '.'){
                    dot_ = 1;
                    digit_ = 1;
                }
                else if(s != '.' && dot_ != 1){
                    ang = ang * 10 +((int)s) - 48;
                }else if(s != '.' && dot_ == 1){
                    ang = ang + (pow(0.1,digit_))*(((int)s) - 48);
                    digit_++;
                }
            }     
            ///////////////////////    
            ThisThread::sleep_for(100ms);
      }

   }

}