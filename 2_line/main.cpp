#include"mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

using namespace std::chrono;


BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
DigitalIn encoder(D11);
volatile int steps;
volatile int last;

BBCar car(pin5, pin6, servo_ticker);

char s, letter;
int i = 0;
int X1 = 0;
int X2 = 0;
int Y1 = 0;
int Y2 = 0;
char recv[1];

Thread t;

int num, m;
double d;
double angle;

void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
}

void cargo(){
    while(1){
        printf("1x1 = %3d, x2 = %3d, Y1 = %3d, Y2 = %3d\n",X1, X2, Y1, Y2);
        ThisThread::sleep_for(1s);

        if(Y2 != 0){
            ThisThread::sleep_for(10ms);
            printf("1x1 = %3d, x2 = %3d, Y1 = %3d, Y2 = %3d\n",X1, X2, Y1, Y2);
            double X = (double)X1-(double)X2;
            double Y = (double)Y2-(double)Y1;
            double P = X/Y;
            printf("X = %f, Y = %f\n", X, Y);
            printf("X/Y = %f\n", P);

            angle = abs((atan(P)*180)/3.14);    // calculate the angle of the line
            printf("angle = %f\n", angle);

            if(X2 > X1)      {       // if car at west, turn left
                for (m = 0; m < 1.25 * angle; m++){
                    car.turn(200, 0.05);
                    ThisThread::sleep_for(10ms);
                }
                car.stop();
            }
            else {                  // if car at east, turn right

                for (m = 0; m < 1.25 * angle; m++){
                    car.turn(200, -0.05);
                    ThisThread::sleep_for(10ms);
                }
                car.stop();
            }
            ThisThread::sleep_for(100ms);
            printf("end turn!!!\n");
    
            ///////calculate the line length///////////
            d = sqrt((Y1- Y2)*(Y1- Y2) + (X1- X2) * (X1- X2));

            while(steps*6.5*3.14/32 < d) {  // walk back for the length
                car.goStraight(-200);
                ThisThread::sleep_for(100ms);
            }
            car.stop();
            return;
        }
    }
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

            if(s == 'x'){
                i++;
            }else if (s == 'y'){
                i++;
            }else if (s == '='){
                i++;
            }
            
            ///////store X1, X2, Y1, Y2 from openMV//////////
            if(i == 2 && isdigit(recv[0]) == 1){
                X1 = X1 * 10 +((int)s) - 48;
            }else if (i == 4 && isdigit(recv[0]) == 1){
                Y1 = Y1 * 10 +((int)s) - 48;
            }else if (i == 6 && isdigit(recv[0]) == 1){
                X2 = X2 * 10 +((int)s) - 48;
            }else if (i == 8 && isdigit(recv[0]) == 1){
                Y2 = Y2 * 10 +((int)s) - 48;
            }                
            ThisThread::sleep_for(100ms);
      }

   }

}