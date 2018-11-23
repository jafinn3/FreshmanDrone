//This file is the updated Master file for the software which runs the drone.
//Credits:


//Importing libraries necessary
#include <SPI.h>
#include <RH_RF69.h>
#include <Servo.h> // Look at the readme file before running


//Defining the objects in the scope
Servo motA, motB, motC, motD; // Creating the four servos we will use on the drone

/************ Servo Setup ***************/

//We might be able to change this? Look into this!! If someone sees this, please remind me!
#define MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs
#define MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs

#define SERVO_PIN_1      46
#define SERVO_PIN_2      47
#define SERVO_PIN_3      48
#define SERVO_PIN_4      49

/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ     915.0
#define RFM69_CS      53 // This is the CS port on the radio
#define RFM69_INT     3  // Need to ask Brian/Joe what these two are
#define RFM69_RST     2

#define BUFF_SIZE     5  // Number of bytes we will be receiving from the other radio at a time

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);




/*********** INTEGRATING NEW CODE ***********/

//thrust corresponds to a CHANGE in motor speed
int thrust1 = 0;
int thrust2 = 0;
int thrust3 = 0;
int thrust4 = 0;
//adjust to JS
const int highBreak = 525;
const int lowBreak = 475;
//adjust to copter
const int hoverSpeed = 50;
const int maxSpeed = 1000;
const int range = 100;

void setup() 
{
  //Setting up the serial
  Serial.begin(115200);

  //Attaching the motors to their pins
  delay(3000);
  motA.attach(SERVO_PIN_2, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motB.attach(SERVO_PIN_1, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motC.attach(SERVO_PIN_3, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motD.attach(SERVO_PIN_4, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  
  delay(1000);
  
  
  //Initializing the radio
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);
  
  //Resetting the radio
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
    //Blink the LED on the arduino to indicate that this is broken
  }
  
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW
  
  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  rf69.setEncryptionKey(key);
  
  //Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

int adjustValue(int value, int highConstraint, int lowConstraint){
  if(value > highConstraint)
    return highConstraint;
  if(value < lowConstraint)
    return lowConstraint;
  return value;
}

void loop() {
  //Checking if there is something available for us to read
  if (rf69.available()) {
    //Initializing the buffer(Bytes of information being sent)
    uint8_t buff[BUFF_SIZE];
    uint8_t len = BUFF_SIZE;

    //
    if (rf69.recv(buff, &len)) {
      if (!len) return; // I believe we can remove this sine we are always sending 5 bytes
      //Serial.print("Received [");
      //Serial.print(len);
      //Serial.print("]: ");
      
      int joystickValues[4]; //Initializing the int[] which will store the values we encoded in 10 bits

      // Getting the actual values from the 40 bits that were sent. Need to shift the values appropriately to get the currect bits.
      joystickValues[0] = (int)((((uint16_t)(buff[0]) << 2) & 0x3FF) | (buff[1] >> 6)); //RightX Joystick
      joystickValues[1] = (int)((((uint16_t)(buff[1]) << 4) & 0x3FF) | (buff[2] >> 4)); //RightY Joystick
      joystickValues[2] = (int)((((uint16_t)(buff[2]) << 6) & 0x3FF) | (buff[3] >> 2)); //LeftX Joystick
      joystickValues[3] = (int)((((uint16_t)(buff[3]) << 8) & 0x3FF) | (buff[4]));      //LeftY Joystick

      Serial.print("Right X: ");
      Serial.println(joystickValues[0]);
      Serial.print("Right Y: ");
      Serial.println(joystickValues[1]);
      Serial.print("Left X: ");
      Serial.println(joystickValues[2]);
      Serial.print("Left Y: ");
      Serial.println(joystickValues[3]);

      //The values being sent across in the joystickValues array must be between 0 - 1023
      //If they are not, we will change the values accordingly
      joystickValues[0] = adjustValue(joystickValues[0], 1023, 0);
      joystickValues[1] = adjustValue(joystickValues[1], 1023, 0);
      joystickValues[2] = adjustValue(joystickValues[2], 1023, 0);
      joystickValues[3] = adjustValue(joystickValues[3], 1023, 0);
      
      //Joystick Software
      int RightXVal = joystickValues[0];
      int RightYVal = joystickValues[1];
      int LeftXVal = joystickValues[2];
      int LeftYVal = joystickValues[3];

      //Initialize Hover
      thrust1 = 0;
      thrust2 = 0;
      thrust3 = 0;
      thrust4 = 0;
    
      //LEFT JOYSTICK
      //Climb
      if(LeftYVal >= highBreak)
      {
        thrust1 = map(LeftYVal, highBreak, 1023, 0, range);
        thrust2 = map(LeftYVal, highBreak, 1023, 0, range);
        thrust3 = map(LeftYVal, highBreak, 1023, 0, range);
        thrust4 = map(LeftYVal, highBreak, 1023, 0, range);
        Serial.println("Climb");
      }
      //Descend
      else if(LeftYVal <= lowBreak)
      {
        thrust1 = -map(LeftYVal, 0, lowBreak, range, 0);
        thrust2 = -map(LeftYVal, 0, lowBreak, range, 0);
        thrust3 = -map(LeftYVal, 0, lowBreak, range, 0);
        thrust4 = -map(LeftYVal, 0, lowBreak, range, 0);
        Serial.println("Descend");
      }
      //Yaw R
      else if(LeftXVal >= highBreak)
      {
        thrust1 = map(LeftXVal, highBreak, 1023, 0, range);
        thrust2 = -map(LeftXVal, highBreak, 1023, 0, range);
        thrust3 = map(LeftXVal, highBreak, 1023, 0, range);
        thrust4 = -map(LeftXVal, highBreak, 1023, 0, range);;
        Serial.println("Yaw R");
      }
      //Yaw L
      else if(LeftXVal <= lowBreak)
      {
        thrust2 = map(LeftXVal, 0, lowBreak, range, 0);
        thrust1 = -map(LeftXVal, 0, lowBreak, range, 0);
        thrust3 = -map(LeftXVal, 0, lowBreak, range, 0);
        thrust4 = map(LeftXVal, 0, lowBreak, range, 0);
        Serial.println("Yaw L");
      }

      //Check the left joystick for movement, if there is something outside the deadzone, we will take that raw value, then check the right joystick
      //Now when we are checking the right joystick, we will look to see if the left joystick was outside the deadzone by seeing if the trust has changed(Originally 0)
      //If the thrust didn't change, then we want to set the thrust to the raw value of the right joystick,
      //If the trust did change in the left joystick, then we want to set the thrust to .5 left + .5 right raw values so that we never go over the max range

      //RIGHT JOYSTICK
      //Pitch FWD
      if(RightYVal >= highBreak)
      {
        if(thrust1 == 0){
          thrust3 = map(RightYVal, highBreak, 1023, 0, range);
          thrust1 = -map(RightYVal, highBreak, 1023, 0, range);
          thrust2 = -map(RightYVal, highBreak, 1023, 0, range);
          thrust4 = map(RightYVal, highBreak, 1023, 0, range);
        }else{
          thrust3 = .5*(thrust3) + .5*(map(RightYVal, highBreak, 1023, 0, range));
          thrust1 = .5*(thrust1) + .5*(-map(RightYVal, highBreak, 1023, 0, range));
          thrust2 = .5*(thrust2) + .5*(-map(RightYVal, highBreak, 1023, 0, range));
          thrust4 = .5*(thrust4) + .5*(map(RightYVal, highBreak, 1023, 0, range));
        }
        Serial.println("Pitch FWD");
      }
      //Pitch REV
      else if(RightYVal <= lowBreak)
      {
        if(thrust1 == 0){
          thrust1 = map(RightYVal, 0, lowBreak, range, 0);
          thrust2 = map(RightYVal, 0, lowBreak, range, 0);
          thrust3 = -map(RightYVal, 0, lowBreak, range, 0);
          thrust4 = -map(RightYVal, 0, lowBreak, range, 0);
        }else{
          thrust1 = .5*(thrust1) + .5*(map(RightYVal, 0, lowBreak, range, 0));
          thrust2 = .5*(thrust2) + .5*(map(RightYVal, 0, lowBreak, range, 0));
          thrust3 = .5*(thrust3) + .5*(-map(RightYVal, 0, lowBreak, range, 0));
          thrust4 = .5*(thrust4) + .5*(-map(RightYVal, 0, lowBreak, range, 0));
        }
        Serial.println("Pitch REV");
      }
      //Roll R
      else if(RightXVal >= highBreak)
      {
        if(thrust1 == 0){
          thrust1 = map(RightXVal, highBreak, 1023, 0, range);
          thrust2 = -map(RightXVal, highBreak, 1023, 0, range);
          thrust3 = -map(RightXVal, highBreak, 1023, 0, range);
          thrust4 = map(RightXVal, highBreak, 1023, 0, range);
        }else{
          thrust1 = .5*(thrust1) + .5*(map(RightXVal, highBreak, 1023, 0, range));
          thrust2 = .5*(thrust2) + .5*(-map(RightXVal, highBreak, 1023, 0, range));
          thrust3 = .5*(thrust3) + .5*(-map(RightXVal, highBreak, 1023, 0, range));
          thrust4 = .5*(thrust4) + .5*(map(RightXVal, highBreak, 1023, 0, range));
        }
        Serial.println("Roll R");
      }
      //ROll L
      else if(RightXVal <= lowBreak)
      {
        if(thrust1 == 0){
          thrust2 = map(RightXVal, 0, lowBreak, range, 0);
          thrust1 = -map(RightXVal, 0, lowBreak, range, 0);
          thrust3 = map(RightXVal, 0, lowBreak, range, 0);
          thrust4 = -map(RightXVal, 0, lowBreak, range, 0);
        }else{
          thrust2 = .5*(thrust2) + .5*(map(RightXVal, 0, lowBreak, range, 0));
          thrust1 = .5*(thrust1) + .5*(-map(RightXVal, 0, lowBreak, range, 0));
          thrust3 = .5*(thrust3) + .5*(map(RightXVal, 0, lowBreak, range, 0));
          thrust4 = .5*(thrust4) + .5*(-map(RightXVal, 0, lowBreak, range, 0));
        }
        Serial.println("Roll L");
      }
      //Insert accelerometer/gyro adjustments
      thrust1 = 1000 + hoverSpeed + thrust1;
      thrust2 = 1000 + hoverSpeed + thrust2;
      thrust3 = 1000 + hoverSpeed + thrust3;
      thrust4 = 1000 + hoverSpeed + thrust4;

      thrust1 = adjustValue(thrust1, 2000, 1000);
      thrust2 = adjustValue(thrust2, 2000, 1000);
      thrust3 = adjustValue(thrust3, 2000, 1000);
      thrust4 = adjustValue(thrust4, 2000, 1000);

      
      Serial.print("Motor 1:");
      Serial.println(thrust1);
      Serial.print("Motor 2: ");
      Serial.println(thrust2);
      Serial.print("Motor 3: ");
      Serial.println(thrust3);
      Serial.print("Motor 4: ");
      Serial.println(thrust4);

      //Writing the values to the motors
      motA.writeMicroseconds(thrust1);
      motB.writeMicroseconds(thrust2);
      motC.writeMicroseconds(thrust3);
      motD.writeMicroseconds(thrust4);
      
      Serial.print("Radio Strength: ");
      Serial.println(rf69.lastRssi(), DEC);

      //Sending a response back to the controller( The response will be one character, R --> Received )
//      uint8_t data[] = "R";
//      rf69.send(data, sizeof(data));
//      rf69.waitPacketSent();
//      Serial.println("Response just sent");      
    } else {
      Serial.println("Receive failed");
    }
  }
}
