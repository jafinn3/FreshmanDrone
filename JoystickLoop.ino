//Joystick Software

//X and Y pins on Arduino and Joystick are switched (VRx = y-axis, VRy = x-axis)
//Right Joystick
const int Right_X_pin = A0;
const int Right_Y_pin = A1;

//Left Joystick
const int Left_X_pin = A2;
const int Left_Y_pin = A3;

const int led4L = 2;
const int led4H = 3;
const int led3H = 4;
const int led3L = 5;
const int led1L = 6;
const int led1H = 7;
const int led2H = 8;
const int led2L = 9;

//Joystick Button
const int buttonPin = 13;

//thrust corresponds to a CHANGE in motor speed
int thrust1 = 0;
int thrust2 = 0;
int thrust3 = 0;
int thrust4 = 0;
//adjust to JS
const int highBreak = 525;
const int lowBreak = 475;
//adjust to copter
const int hoverThrust = 750;
const int maxThrust = 1000;
const int range = min(hoverThrust, maxThrust - hoverThrust);

void setup() {
  Serial.begin(9600);
  //Joystick Initialization
  //Right JS
  pinMode(Right_X_pin, INPUT);
  pinMode(Right_Y_pin, INPUT);
  //Left JS
  pinMode(Left_X_pin, INPUT);
  pinMode(Left_Y_pin, INPUT);

  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  //Joystick Software
  int RightXVal = analogRead(Right_X_pin);
  int RightYVal = analogRead(Right_Y_pin);
  int LeftXVal = analogRead(Left_X_pin);
  int LeftYVal = analogRead(Left_Y_pin);

  //LEFT JOYSTICK
  //Climb
  if(LeftYVal >= highBreak)
  {
    thrust1 = map(LeftYVal, highBreak, 1023, 0, range);
    thrust2 = thrust1;
    thrust3 = thrust1;
    thrust4 = thrust1;
    digitalWrite(led1H,HIGH);
    digitalWrite(led1L,LOW);
    digitalWrite(led2H,HIGH);
    digitalWrite(led2L,LOW);
    digitalWrite(led3H,HIGH);
    digitalWrite(led3L,LOW);
    digitalWrite(led4H,HIGH);
    digitalWrite(led4L,LOW);
    Serial.println("Climb");
  }
  //Descend
  else if(LeftYVal <= lowBreak)
  {
    thrust1 = -map(LeftYVal, 0, lowBreak, range, 0);
    thrust2 = thrust1;
    thrust3 = thrust1;
    thrust4 = thrust1;
    digitalWrite(led1H,LOW);
    digitalWrite(led1L,HIGH);
    digitalWrite(led2H,LOW);
    digitalWrite(led2L,HIGH);
    digitalWrite(led3H,LOW);
    digitalWrite(led3L,HIGH);
    digitalWrite(led4H,LOW);
    digitalWrite(led4L,HIGH);
    Serial.println("Descend");
  }
  //Yaw R
  else if(LeftXVal >= highBreak)
  {
    thrust1 = map(LeftXVal, highBreak, 1023, 0, range);
    thrust2 = -thrust1;
    thrust3 = thrust1;
    thrust4 = thrust2;
    digitalWrite(led1H,HIGH);
    digitalWrite(led1L,LOW);
    digitalWrite(led2H,LOW);
    digitalWrite(led2L,HIGH);
    digitalWrite(led3H,HIGH);
    digitalWrite(led3L,LOW);
    digitalWrite(led4H,LOW);
    digitalWrite(led4L,HIGH);
    Serial.println("Yaw R");
  }
  //Yaw L
  else if(LeftXVal <= lowBreak)
  {
    thrust2 = map(LeftXVal, 0, lowBreak, range, 0);
    thrust1 = -thrust2;
    thrust3 = thrust1;
    thrust4 = thrust2;
    digitalWrite(led1H,LOW);
    digitalWrite(led1L,HIGH);
    digitalWrite(led2H,HIGH);
    digitalWrite(led2L,LOW);
    digitalWrite(led3H,LOW);
    digitalWrite(led3L,HIGH);
    digitalWrite(led4H,HIGH);
    digitalWrite(led4L,LOW);
    Serial.println("Yaw L");
  }

  //RIGHT JOYSTICK
  //Pitch FWD
  if(RightYVal >= highBreak)
  {
    thrust3 = map(RightYVal, highBreak, 1023, 0, range);
    thrust1 = -thrust3;
    thrust2 = thrust1;
    thrust4 = thrust3;
    digitalWrite(led1H,LOW);
    digitalWrite(led1L,HIGH);
    digitalWrite(led2H,LOW);
    digitalWrite(led2L,HIGH);
    digitalWrite(led3H,HIGH);
    digitalWrite(led3L,LOW);
    digitalWrite(led4H,HIGH);
    digitalWrite(led4L,LOW);
    Serial.println("Pitch FWD");
  }
  //Pitch REV
  else if(RightYVal <= lowBreak)
  {
    thrust1 = map(RightYVal, 0, lowBreak, range, 0);
    thrust2 = thrust1;
    thrust3 = -thrust1;
    thrust4 = thrust3;
    digitalWrite(led1H,HIGH);
    digitalWrite(led1L,LOW);
    digitalWrite(led2H,HIGH);
    digitalWrite(led2L,LOW);
    digitalWrite(led3H,LOW);
    digitalWrite(led3L,HIGH);
    digitalWrite(led4H,LOW);
    digitalWrite(led4L,HIGH);
    Serial.println("Pitch REV");
  }
  //Roll R
  else if(RightXVal >= highBreak)
  {
    thrust1 = map(RightXVal, highBreak, 1023, 0, range);
    thrust2 = -thrust1;
    thrust3 = thrust2;
    thrust4 = thrust1;
    digitalWrite(led1H,HIGH);
    digitalWrite(led1L,LOW);
    digitalWrite(led2H,LOW);
    digitalWrite(led2L,HIGH);
    digitalWrite(led3H,LOW);
    digitalWrite(led3L,HIGH);
    digitalWrite(led4H,HIGH);
    digitalWrite(led4L,LOW);
    Serial.println("Roll R");
  }
  //ROll L
  else if(RightXVal <= lowBreak)
  {
    thrust2 = map(RightXVal, 0, lowBreak, range, 0);
    thrust1 = -thrust2;
    thrust3 = thrust2;
    thrust4 = thrust1;
    digitalWrite(led1H,LOW);
    digitalWrite(led1L,HIGH);
    digitalWrite(led2H,HIGH);
    digitalWrite(led2L,LOW);
    digitalWrite(led3H,HIGH);
    digitalWrite(led3L,LOW);
    digitalWrite(led4H,LOW);
    digitalWrite(led4L,HIGH);
    Serial.println("Roll L");
  }

  else
  {
    thrust1 = 0;
    thrust2 = thrust1;
    thrust3 = thrust1;
    thrust4 = thrust1;
    digitalWrite(led1H,LOW);
    digitalWrite(led1L,LOW);
    digitalWrite(led2H,LOW);
    digitalWrite(led2L,LOW);
    digitalWrite(led3H,LOW);
    digitalWrite(led3L,LOW);
    digitalWrite(led4H,LOW);
    digitalWrite(led4L,LOW);
    Serial.println("Hover");
  }
  /*
  Serial.print("Thrust 1 = ");
  Serial.println(thrust1);
  Serial.print("Thrust 2 = ");
  Serial.println(thrust2);
  Serial.print("Thrust 3 = ");
  Serial.println(thrust3);
  Serial.print("Thrust 4 = ");
  Serial.println(thrust4);
  */
  //Configure JS axes
  /*
  int Lx = analogRead(Left_X_pin);
  int Ly = analogRead(Left_Y_pin);
  Serial.print("Lx = ");
  Serial.println(Lx);
  Serial.print("Ly = ");
  Serial.println(Ly);
  */
  
  //Serial.println(!digitalRead(buttonPin));
  //delay(1000);
}
