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


void setup() 
{
  //Setting up the serial
  Serial.begin(115200);

  //Attaching the motors to their pins
  delay(3000);
  motA.attach(SERVO_PIN_1, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motB.attach(SERVO_PIN_2, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
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

int adjustValue(int value){
  if(value > 1000)
    return 1000;
  if(value < 0)
    return 0;
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
      joystickValues[0] = (int)((((uint16_t)(buff[0]) << 2) & 0x3FF) | (buff[1] >> 6)); //LeftX Joystick
      joystickValues[1] = (int)((((uint16_t)(buff[1]) << 4) & 0x3FF) | (buff[2] >> 4)); //LeftY Joystick
      joystickValues[2] = (int)((((uint16_t)(buff[2]) << 6) & 0x3FF) | (buff[3] >> 2)); //RightX Joystick
      joystickValues[3] = (int)((((uint16_t)(buff[3]) << 8) & 0x3FF) | (buff[4]));      //RightY Joystick

      Serial.print("Left X: ");
      Serial.println(joystickValues[0]);
      Serial.print("Left Y: ");
      Serial.println(joystickValues[1]);
      Serial.print("Right X: ");
      Serial.println(joystickValues[2]);
      Serial.print("Right Y: ");
      Serial.println(joystickValues[3]);

      //The values being sent across in the joystickValues array must be between 0 - 1000
      //If they are not, we will change the values accordingly
      joystickValues[0] = adjustValue(joystickValues[0]);
      joystickValues[1] = adjustValue(joystickValues[1]);
      joystickValues[2] = adjustValue(joystickValues[2]);
      joystickValues[3] = adjustValue(joystickValues[3]);

      //Insert accelerometer/gyro adjustments


      //Writing the values to the motors
//      motA.writeMicroseconds(1000 + joystickValues[0]);
//      motB.writeMicroseconds(1000 + joystickValues[1]);
//      motC.writeMicroseconds(1000 + joystickValues[2]);
//      motD.writeMicroseconds(1000 + joystickValues[3]);
//      
//      delay(1000);
      
      
      Serial.print("Radio Strength: ");
      Serial.println(rf69.lastRssi(), DEC);

      //Sending a response back to the controller( The response will be one character, R --> Received )
      uint8_t data[] = "R";
      rf69.send(data, sizeof(data));
      rf69.waitPacketSent();
      Serial.println("Response just sent");      
    } else {
      Serial.println("Receive failed");
    }
  }
}
