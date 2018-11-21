// Include the RFM69 and SPI libraries:
#include <RFM69.h>
#include <SPI.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     100   // Must be the same for all nodes (0 to 255)
#define MYNODEID      2  // My node ID (0 to 255)
#define TONODEID      1   // Destination node ID (0 to 254, 255 = broadcast)

// RFM69 frequency, uncomment the frequency of your module:
//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):
#define ENCRYPT       false // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):
#define USEACK        false// Request ACKs or not

// Create a library object for our RFM69HCW module:
RFM69 radio;




#include <Servo.h>
// ---------------------------------------------------------------------------
// Customize here pulse lengths as needed
#define MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs
#define MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs
// ---------------------------------------------------------------------------
Servo motA, motB, motC, motD;
char data;
int c = 1000;
// ---------------------------------------------------------------------------

void setup()
{
  //COMMUNICATIONS
  // Open a serial port so we can send keystrokes to the module:
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");  
  
  // Initialize the RFM69HCW:
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW

  // Turn on encryption if desired:
  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);



  //MOTORS AND ESC

  motA.attach(46, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motB.attach(47, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motC.attach(48, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motD.attach(49, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
  motA.writeMicroseconds(1000);
  motB.writeMicroseconds(1000);
  motC.writeMicroseconds(1000);
  motD.writeMicroseconds(1000);
}

void loop()
{
  // Set up a "buffer" for characters that we'll send:
  static char sendbuffer[61];
  static int sendlength = 0;
  
  // RECEIVING
  // In this section, we'll check with the RFM69HCW to see
  // if it has received any packets:
  if (radio.receiveDone()) // Got one!
  {
    Serial.print("In Recieved!");
    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:
    int characterArray[4];
    //TWELVE CHARACTERS ( 4 VALUES OF 3 DIGITS(0-999) )

    for(int i = 0; i < 4; i++){
      String tempString = "";
      for (byte j = 0; j < 3; j++){
        tempString += (char)radio.DATA[i*3 + j];
      }
      characterArray[i] = tempString.toInt();
    }

    motA.writeMicroseconds(1000 + characterArray[0]);
    motB.writeMicroseconds(1000 + characterArray[1]);
    motC.writeMicroseconds(1000 + characterArray[2]);
    motD.writeMicroseconds(1000 + characterArray[3]);

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)
    if (radio.ACKRequested())
    {
      radio.sendACK();
    }
  }
}
