// Include the RFM69 and SPI libraries:
#include <RFM69.h>
#include <SPI.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     100   // Must be the same for all nodes (0 to 255)
#define MYNODEID      2   // My node ID (0 to 255)
#define TONODEID      1   // Destination node ID (0 to 254, 255 = broadcast)

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

void setup()
{
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
}

void loop()
{
  // Set up a "buffer" for characters that we'll send:
  static char sendbuffer[61];
  static int sendlength = 0;
  
  // SENDING
  // In this section, we'll gather serial characters and
  // send them to the other node if we (1) get a carriage return,
  // or (2) the buffer is full (61 characters).
  
  // If there is any serial input, add it to the buffer:
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    
    if (input != '\r') // not a carriage return
    {
      sendbuffer[sendlength] = input;
      sendlength++;
    }

    // If the input is a carriage return, or the buffer is full:
    
    if ((input == '\r') || (sendlength == 5)) // CR or buffer full
    {
      // Send the packet!
      Serial.print("sending to node ");
      Serial.print(TONODEID, DEC);
      Serial.print(": [");
      for (byte i = 0; i < sendlength; i++)
        Serial.print(sendbuffer[i]);
      Serial.println("]");
      
      // There are two ways to send packets. If you want
      // acknowledgements, use sendWithRetry():
      if (USEACK)
      {
        if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength))
          Serial.println("ACK received!");
        else
          Serial.println("no ACK received :(");
      }

      // If you don't need acknowledgements, just use send():
      else // don't use ACK
      {
        radio.send(TONODEID, sendbuffer, sendlength);
      }
      
      sendlength = 0; // reset the packet
    }
  }
}
