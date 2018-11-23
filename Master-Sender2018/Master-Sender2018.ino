// rf69 demo tx rx.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem 
// configuration

#include <SPI.h>
#include <RH_RF69.h>


/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0

#define RFM69_INT     3  // 
#define RFM69_CS      10  //
#define RFM69_RST     2  // "A"

#define NUM_BYTES 5


// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission

//X and Y pins on Arduino and Joystick are switched (VRx = y-axis, VRy = x-axis)
//Right Joystick
const int Right_X_pin = A1;
const int Right_Y_pin = A2;

//Left Joystick
const int Left_X_pin = A6;
const int Left_Y_pin = A7;

void setup() 
{
  Serial.begin(9600);
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer

  //Joystick Initialization
  //Right JS
  pinMode(Right_X_pin, INPUT);
  pinMode(Right_Y_pin, INPUT);
  //Left JS
  pinMode(Left_X_pin, INPUT);
  pinMode(Left_Y_pin, INPUT);
   
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 TX Test!");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);
  
  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
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

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}



void loop() {
  //delay(1000);  // Wait 1 second between transmits, could also 'sleep' here!

  static char sendbuffer[61];
  static int js[4];
  static int jsLength = 0;
  static int sendlength = 0;

  //Joystick Software
  int RightXVal = analogRead(Right_X_pin);
  int RightYVal = analogRead(Right_Y_pin);
  int LeftXVal = analogRead(Left_X_pin);
  int LeftYVal = analogRead(Left_Y_pin);
  
  js[0] = RightXVal;
  js[1] = RightYVal;
  js[2] = LeftXVal;
  js[3] = LeftYVal;

  Serial.println(js[0]);
  Serial.println(js[1]);
  Serial.println(js[2]);
  Serial.println(js[3]);


  uint8_t radioTransfer[NUM_BYTES];
  radioTransfer[0] = (uint8_t)((js[0] >> 2));
  radioTransfer[1] = (uint8_t)((js[0] << 6)|(js[1] >> 4));
  radioTransfer[2] = (uint8_t)((js[1] << 4)|(js[2] >> 6));
  radioTransfer[3] = (uint8_t)((js[2] << 2)|(js[3] >> 8));
  radioTransfer[4] = (uint8_t)(js[3]);


//  char radiopacket[20] = "Hello World #";
//  itoa(packetnum++, radiopacket+13, 10);
//  Serial.print("Sending "); Serial.println(radiopacket);
  
  // Send a message!
  rf69.send((uint8_t *)radioTransfer, strlen(NUM_BYTES));
  rf69.waitPacketSent();
  Serial.println("Sent.");

  delay(2000);

  /*
  // Now wait for a reply
  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf69.waitAvailableTimeout(500))  { 
    // Should be a reply message for us now   
    if (rf69.recv(buf, &len)) {
      Serial.print("Got a reply: ");
      Serial.println((char*)buf);
    } else {
      Serial.println("Receive failed");
    }
  } else {
    Serial.println("No reply, is another RFM69 listening?");
  }
  */
}
