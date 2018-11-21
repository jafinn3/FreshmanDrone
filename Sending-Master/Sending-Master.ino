//Sending - Master Code for Drone Project (Winter Break 2017)//

//Joystick Software//
const int X_pin = 0;
const int Y_pin = 1;

//Communications Software//
// Include the RFM69 and SPI libraries:
#include <RFM69.h>
#include <SPI.h>
// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     100   // Must be the same for all nodes (0 to 255)
#define MYNODEID      1   // My node ID (0 to 255)
#define TONODEID      2   // Destination node ID (0 to 254, 255 = broadcast)
// RFM69 frequency, uncomment the frequency of your module:
#define FREQUENCY     RF69_915MHZ
// AES encryption (or not):
#define ENCRYPT       false // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes
// Use ACKnowledge when sending messages (or not):
#define USEACK        false// Request ACKs or not
// Create a library object for our RFM69HCW module:
RFM69 radio;

//Joystick Software
String getJSValues(){
  int xVal = analogRead(X_pin);
  int yVal = analogRead(Y_pin);
  String finalStringX = "";
  String finalStringY = "";
  //X Value
  if((486 < xVal) && (xVal < 536))
  {
    finalStringX = "00";
  }
  else if(xVal >= 536)
  {
    int finalValueX = (.205338809)*(xVal - 536);
    if(finalValueX > 85)
      finalValueX = 85;
    finalStringX = String(finalValueX);
  }
  else if(xVal <= 486)
  {
    int finalValueX = (-.2057613169)*(486-xVal);
    finalStringX = String(finalValueX);
  }
  //Y Value
  if((486 < yVal) && (yVal < 536))
  {
    finalStringY = "00";
  }
  else if(yVal >= 536)
  {
    int finalValueY = (.205338809)*(yVal - 536);
    if(finalValueY > 85)
      finalValueY = 85;
    finalStringY = String(finalValueY);
  }
  else if(yVal <= 486)
  {
    int finalValue = (-.2057613169)*(486-yVal);
    finalStringY = String(finalValueY);
  }
  return(finalStringX + finalStringY);  
}

void setup() {
  //Joystick Initialization
  pinMode(X_pin, INPUT);
  pinMode(Y_pin, INPUT);

  //Communications Code
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW 

  // Turn on encryption if desired:
  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
}

void loop() {
  //Joystick Software
  String joystickValues = getJSValues();
  delay(300);

  //Communications Code - Sending JS Values
  static char sendbuffer[61];
  static int sendlength = 0;
  
  String inputStr = joystickValues;
  for(int x = 0; x < inputStr.length();x++)
  {
    char input = inputStr.substring(x,x+1);
    if (input != '\r') // not a carriage return
    {
      sendbuffer[sendlength] = input;
      sendlength++;
    }
    //If the input is a carriage return, or the buffer is full:
    if ((input == '\r') || (sendlength == 4)) // CR or buffer full
    {
      radio.send(TONODEID, sendbuffer, sendlength);
      sendlength = 0; // reset the packet
    }
  }
  
}

