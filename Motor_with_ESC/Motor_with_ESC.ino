#include <Servo.h>

Servo esc;
int throttlePin = 0;
 
void setup()
{
  esc.attach(9);
}
 
void loop()
{
  /*
  int throttle = analogRead(throttlePin);
  throttle = map(throttle, 0, 1023, 0, 179);*/
  int throttle = 10;
  esc.write(throttle);
}
