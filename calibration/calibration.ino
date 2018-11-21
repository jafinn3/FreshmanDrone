#include <Servo.h>

int pos = 0; //Sets position variable

Servo ESC1;
Servo ESC2;
Servo ESC3;
Servo ESC4;

// Include the RFM69 and SPI libraries:
#include <RFM69.h>
#include <SPI.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     100   // Must be the same for all nodes (0 to 255)
#define MYNODEID      1   // My node ID (0 to 255)
#define TONODEID      2   // Destination node ID (0 to 254, 255 = broadcast)

// RFM69 frequency, uncomment the frequency of your module:
//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):
#define ENCRYPT       false // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):
#define USEACK        true// Request ACKs or not

// Create a library object for our RFM69HCW module:
RFM69 radio;


void setup() {
  ESC1.attach(8); //Adds ESC to certain pin. arm();
  ESC2.attach(9); //Adds ESC to certain pin. arm();
  ESC3.attach(10); //Adds ESC to certain pin. arm();
  ESC4.attach(11); //Adds ESC to certain pin. arm();
  setSpeed(0);
  Serial.begin(9600);
  delay(1000);
  int speed; //Implements speed variable
  
  for(speed = 0; speed <= 120; speed += 5) { //Cycles speed up to 70% power for 1 second
  
    setSpeed(speed); //Creates variable for speed to be used in in for loop
    
    delay(1000);
  
  }

  delay(1000); //Stays on for 4 seconds
  Serial.println("High");
  for(speed = 120; speed > 0; speed -= 5) { // Cycles speed down to 0% power for 1 second
  
    setSpeed(speed); 
    delay(1000);
  
  }
  Serial.println("Low");

  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");  
  
  // Initialize the RFM69HCW:
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW

  // Turn on encryption if desired:
  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
}

void loop() {
  static char sendbuffer[61];
  static int sendlength = 0;
  if (Serial.available() > 0) {
    int speed = Serial.parseInt();
    Serial.println(speed);
    Serial.flush();
    setSpeed(speed);
    //delay(10);
    
  }

  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:
    int speed = 0;
    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(": [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:
    for (byte i = 0; i < radio.DATALEN; i++) {
      int dig = (int)radio.DATA[i];
      speed += dig * pow(10, radio.DATALEN - i);
      Serial.print((char)radio.DATA[i]);
      
    }
    Serial.println(speed);
    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.
    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);
    setSpeed(speed);
    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)
    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
  }
}

void setSpeed(int angle){

  //int angle = map(speed, 0, 100, 0, 180); //Sets servo positions to different speeds ESC1.write(angle);
  ESC1.write(angle);
  ESC2.write(angle);
  ESC3.write(angle);
  ESC4.write(angle);
}

