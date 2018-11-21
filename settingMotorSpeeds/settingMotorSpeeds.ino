/**
 * Usage, according to documentation(https://www.firediy.fr/files/drone/HW-01-V4.pdf) : 
 *     1. Plug your Arduino to your computer with USB cable, open terminal, then type 1 to send max throttle to every ESC to enter programming mode
 *     2. Power up your ESCs. You must hear "beep1 beep2 beep3" tones meaning the power supply is OK
 *     3. After 2sec, "beep beep" tone emits, meaning the throttle highest point has been correctly confirmed
 *     4. Type 0 to send min throttle
 *     5. Several "beep" tones emits, which means the quantity of the lithium battery cells (3 beeps for a 3 cells LiPo)
 *     6. A long beep tone emits meaning the throttle lowest point has been correctly confirmed
 *     7. Type 2 to launch test function. This will send min to max throttle to ESCs to test them
 *
 * @author lobodol <grobodol@gmail.com>
 */
// ---------------------------------------------------------------------------
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

/**
 * Initialisation routine
 */
void setup() {
    delay(3000);
    Serial.begin(9600);
    motA.attach(46, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    motB.attach(47, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    motC.attach(48, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
    motD.attach(49, MIN_PULSE_LENGTH, MAX_PULSE_LENGTH);
}

/**
 * Main function
 */
void loop() {
    String a = Serial.readString();
    if(a != "")
      c = a.toInt();

    if(c < 1000)
      c = 1000;
    if(c > 2000)
      c = 2000;
    
    Serial.println(c);
    // VAlUES MUST BE BETWEEN 1000-2000
    motA.writeMicroseconds(c);
    motB.writeMicroseconds(c);
    motC.writeMicroseconds(c);
    motD.writeMicroseconds(c);

    
    delay(1000);
}
