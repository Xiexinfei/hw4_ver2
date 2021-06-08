#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"


BufferedSerial pc(USBTX, USBRX);

Ticker encoder_ticker;
Ticker servo_ticker;

DigitalIn encoder(D11);
volatile int steps;
volatile int last;
PwmOut pin5(D5), pin6(D6);
BufferedSerial xbee(D1, D0);

BBCar car(pin5, pin6, servo_ticker);

void carcon(Arguments *in, Reply *out);
RPCFunction rpccarcon(&carcon, "carcon");
double x, y, z;
int i;


void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
}

/////////////////////control_car///////////////////////
void carcon (Arguments *in, Reply *out)   {

    x = in->getArg<double>();
    y = in->getArg<double>();
    z = in->getArg<double>(); // 1:west 0:east

    pc.set_baud(9600);
   encoder_ticker.attach(&encoder_control, 10ms);
   steps = 0;
   last = 0;

   while(steps*6.5*3.14/32 < 0.83*(x + 7.5)) {  // walk back for d2 + 7.5(half of parking lot width)
         car.goStraight(-200);
      ThisThread::sleep_for(100ms);
   }

   car.stop();

   for(i = 0; i < 6; i++){

    if(z == 1){            // if at west, turn left
    car.turn(85, 0.3);}
    else if(z == 0)        // if at east, turn right
    car.turn(70, -0.3);

    ThisThread::sleep_for(400ms);
    }

   steps = 0;              // reset steps
   last = 0;

   while(steps*6.5*3.14/32 <  0.9*(y - 10 + 18)) { // walk back for d1 + 18(parking lot depth)
      car.goStraight(-100);
      ThisThread::sleep_for(1s);
   }
   car.stop();

}

//////////////////////////////////////////////////////

int main() {

   char buf[256], outbuf[256];
   FILE *devin = fdopen(&xbee, "r");
   FILE *devout = fdopen(&xbee, "w");
   while (1) {
      memset(buf, 0, 256);
      for( int i = 0; ; i++ ) {
         char recv = fgetc(devin);
         if(recv == '\n') {
            printf("\r\n");
            break;
         }
         buf[i] = fputc(recv, devout);
      }
   RPC::call(buf, outbuf);
   }

}


