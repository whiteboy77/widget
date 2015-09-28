//Safety trigger.
//Triggers when G forces are less than a threshold amount defined by Gfactor


//Based on I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
//
/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"
#include "math.h"
#include <Servo.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#define LED_PIN 13 
/**********************************************************************
**  Alter the Gfactor to suit size of drone. Your chance to shine.. :)*
**********************************************************************/
const float Gfactor = 0.7;    /****************************************
***********************************************************************
**********************************************************************/
MPU6050 accelgyro;
Servo myservo;
int16_t ax, ay, az;
float resultant;  // Magnitude of the resultant accelleration
float mavg=16500; //      moving average over
float nsamp=100;   //      this number of samples
const float g=9.81;      //      m /sec^2
float gmult;
float vertAcceln;

bool blinkState = false;
int pos = 0, ch1;    // variable to store the servo position 

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    Serial.begin(9600);
    // Configure pin 4 for input from radio control Rx
    pinMode(4, INPUT);
  
    myservo.attach(9);
    
    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection - debug only. 
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // configure Arduino LED 
    pinMode(LED_PIN, OUTPUT);
    // Get (hopefully stationary!) g-force
    for(int i=0; i<nsamp; i++){
         accelgyro.getAcceleration(&ax, &ay, &az);
         resultant = ((float)ax*ax + (float)ay*ay + (float)az*az);
         resultant = pow(resultant,0.5);
         mavg = mavg + ((resultant - mavg)/nsamp);
    }
    gmult = g/mavg;
    Serial.print("gmult = ");Serial.println(gmult,5);
}

void loop() {
    //Manual Deloyment - Read input from radio receiver and trigger parachute.
    //Comment out if no connection, as noise will cause false triggering.
   /* ch1 = pulseIn(4, HIGH, 25000);
    if (ch1!= 0){
      Serial.print("ch1 = ");Serial.println(ch1);
      if (ch1 < 1500){ 
        parachute();
      }
    }
    */
    //Automatic deployment - read raw accel/gyro measurements from device
    accelgyro.getAcceleration(&ax, &ay, &az);
    resultant = ((float)ax*ax + (float)ay*ay + (float)az*az);
    resultant = pow(resultant,0.5);
    mavg = mavg + ((resultant - mavg)/nsamp);
    vertAcceln = mavg * gmult;
    if(vertAcceln < Gfactor*g){
      Serial.println(vertAcceln);
      blinkState=true;     
      parachute();
   } 
   //delay(1000);   // Temp loop delay
}
    
    

void parachute()
{
    
    digitalWrite(LED_PIN, blinkState);
    myservo.writeMicroseconds(1100);
    Serial.println ("triggered");
    delay(12500);
    // Reset if possible
    myservo.writeMicroseconds(1500);
    blinkState=false;
    digitalWrite(LED_PIN, blinkState);
    Serial.println ("reset");
    
}
