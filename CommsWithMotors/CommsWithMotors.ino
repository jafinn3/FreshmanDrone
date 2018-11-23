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

// ---------------------------------------------------------------------------
#include <Servo.h>
// ---------------------------------------------------------------------------
// Customize here pulse lengths as needed
#define MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs
#define MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs
// ---------------------------------------------------------------------------
Servo motA, motB, motC, motD;
// ---------------------------------------------------------------------------


/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF69_FREQ 915.0

#define RFM69_CS      53
#define RFM69_INT     3
#define RFM69_RST     2

#define BUFF_SIZE 5

// Singleton instance of the radio driver
RH_RF69 rf69(RFM69_CS, RFM69_INT);

int16_t packetnum = 0;  // packet counter, we increment per xmission

void setup() 
{
  Serial.begin(115200);

  delay(3000);
  motA.attach(46, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motB.attach(47, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motC.attach(48, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motD.attach(49, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  
  //while (!Serial) { delay(1); } // wait until serial console is open, remove if not tethered to computer
  
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 RX Test!");
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
 if (rf69.available()) {
    // Should be a message for us now   
    uint8_t buff[BUFF_SIZE];
    uint8_t len = BUFF_SIZE;
    if (rf69.recv(buff, &len)) {
      if (!len) return;
      Serial.print("Received [");
      Serial.print(len);
      Serial.print("]: ");
    
      int A[4];

      A[0] = (int)((((uint16_t)(buff[0]) << 2) & 0x3FF) | (buff[1] >> 6));
      A[1] = (int)((((uint16_t)(buff[1]) << 4) & 0x3FF) | (buff[2] >> 4));
      A[2] = (int)((((uint16_t)(buff[2]) << 6) & 0x3FF) | (buff[3] >> 2));
      A[3] = (int)((((uint16_t)(buff[3]) << 8) & 0x3FF) | (buff[4]));

      
      
      // VAlUES MUST BE BETWEEN 1000-2000
      Serial.println(A[0]);
      Serial.println(A[1]);
      Serial.println(A[2]);
      Serial.println(A[3]);
      motA.writeMicroseconds(1000 + A[0]);
      motB.writeMicroseconds(1000 + A[1]);
      motC.writeMicroseconds(1000 + A[2]);
      motD.writeMicroseconds(1000 + A[3]);

  
      delay(1000);

      
      Serial.print("RSSI: ");
      Serial.println(rf69.lastRssi(), DEC);
      
      if (strstr((char *)buff, "Hello World")) {
        // Send a reply!
        uint8_t data[] = "And hello back to you";
        rf69.send(data, sizeof(data));
        rf69.waitPacketSent();
        Serial.println("Sent a reply");
      }
    } else {
      Serial.println("Receive failed");
    }
  }
}
