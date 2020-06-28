#include "mbed.h"
#include "bbcar.h"

DigitalOut led3(LED3);
DigitalOut led2(LED2);
DigitalOut led1(LED1);

PwmOut pin9(D9), pin8(D8);

DigitalInOut pin10(D10);
DigitalIn pin3(D3);
Serial uart(D1,D0); // tx,rx OpenMV
Serial pc(USBTX,USBRX); // tx,rx 
Serial xbee(D12, D11); // Xbee

Ticker servo_ticker;
Ticker encoder_ticker;

BBCar car(pin8, pin9, servo_ticker);

float data[50]; 

parallax_encoder encoder0(pin3, encoder_ticker);

 
void straight(int speed, int len)
{
    encoder0.reset();
    car.goStraight(-speed);
    while (encoder0.get_cm() < len)
        wait_ms(50);
    car.stop();
    wait(0.5);
}

void left0()
{
    car.turn(62, 0.1); //turn left
    wait(1.58);//right back
    car.stop();
    wait(0.5);
}
void left1()
{
    car.turn(-63, -0.1); //turn left /right turn get 10 time + left - right
    wait(1.58);//right front
    car.stop();
    wait(0.5);
}

void right0()
{
    car.turn(-60, 0.1); //turn right
    wait(1.57);//left front
    car.stop();
    wait(0.5);
}
void right1()
{
    car.turn(-65, 0.1); //turn right
    wait(1.58);
    car.stop();
    wait(0.5);
}
void right2()
{
    car.turn(62, 0.1); //turn right
    wait(1.58);//left back
    car.stop();
    wait(0.5);
}
void right3()
{
    car.turn(-32, 0.1); //turn right
    wait(1.58);//left front
    car.stop();
    wait(0.5);
}
void right4()
{
    car.turn(43, 0.1); //turn right
    wait(1);//left back
    car.stop();
    wait(0.5);
}
void right5()
{
    car.turn(-50, 0.1); //turn right
    wait(1.58);//left back
    car.stop();
    wait(0.5);
}

 
int main()
{

    xbee.printf("start!\n\r");  
    parallax_ping ping1(pin10);
    int i=0;

     double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table0[] = {-16.588, -16.189, -14.993, -11.803, -6.061, 0.080, 6.061, 11.803, 14.993, 16.189, 16.588};//right
    double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table1[] = {-16.668, -16.109, -14.913, -11.484, -5.503, 0.000, 5.503, 11.484, 14.913, 16.109, 16.668};//left




    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

    //set led
    led1 = 1;
    led2 = 1;
    led3 = 1;

    //first straight
    straight(80, 115);
    xbee.printf("straight\n\r");

    //to parking area
    left1();
    xbee.printf("left\n\r");
    //mission 1
    
    // set led
    led1 = 0;
    led2 = 1;
    led3 = 1;
    
    //go to parking area 1 t0 take a picture
    straight(80, 50);
    xbee.printf("straight\n\r");

    //take picture
    right5();
    xbee.printf("right\n\r");

    straight(-80, 13);
    xbee.printf("straight\n\r");
    //car.stop();

    wait(0.5);
    char s[21];
    sprintf(s,"image_classification"); // sent the command to OpenMV for taking picture
    uart.puts(s); // sent the command by UART
    wait(3);

    straight(80, 10);
    xbee.printf("straight\n\r");

    //turn reverse side
    right2();
    xbee.printf("right\n\r");

    //go to paking area 2
    straight(80, 15);
    xbee.printf("straight\n\r");

    //parking
    right0();
    xbee.printf("right\n\r");

    straight(-80, 30);
    xbee.printf("straight\n\r");

    //leave parking spot 
    straight(80, 28);
    xbee.printf("straight\n\r");


    //turn right and go straight
    right0();
    xbee.printf("right\n\r");

    straight(80, 45);
    xbee.printf("straight\n\r");
    //middle straight

    right5();
    xbee.printf("right\n\r");

    straight(80, 110);
    xbee.printf("straight\n\r");

    right0();
    xbee.printf("right\n\r");

    //mission 2

    //set led
    led1 = 1;
    led2 = 0;
    led3 = 1;

    //go straight to item 2
    straight(80, 48);
    xbee.printf("straight\n\r");

    right0();
    xbee.printf("right\n\r");

    straight(80, 25);
    xbee.printf("straight\n\r");

    //right3();
    //right4();

    //car.stop();

    //save the ping data
    for(i=0;i<50;i++)
    {
        car.turn(-32, 0.1); //turn right
        data[i] = ping1;
        wait(0.02);
    }

    right4();
    xbee.printf("right\n\r");

    straight(-80, 30);
    xbee.printf("straight\n\r");

    left1();
    xbee.printf("left\n\r");

    straight(80, 22);
    xbee.printf("straight\n\r");

    right0();
    xbee.printf("right\n\r");
 
    //final
    //set led
    led1 = 1;
    led2 = 1;
    led3 = 0;

    // final straight
    straight(80, 130);
    xbee.printf("straight\n\r");

     int flag = 0;
     int flags = 0;
    for(i=0;i<50;i++)
    {
        xbee.printf("%f\n\r",data[i]);
        wait(0.1);
        //if(data[i]>14) flag=1;
    }
    float a[50];
    for(i=0;i<50;i++)
    {
        a[i] = data[i+1] - data[i];
    }
    for(i=0;i<50;i++) 
    { 
        if(a[i] * a[i+1] == 0){
            flag = 0;  
        }
        else if(a[i] * a[i+1] > 0){
            flag = 1;
        }
        else if(a[i] * a[i+1] < 0){
            flag = 2;
        }
        else flag = 3;
    }
    if(flags == 0){
        xbee.printf("triangle");
    }
    else if(flags = 1){
        xbee.printf("right triangle");
    }
    else if(flags == 2){
        xbee.printf("other");
    }
    else{
        xbee.printf("rectangle");
    }

}
