/*
 * Project: Midterm Project
 * Description: Smart Room Controller
 * Author: Janel Sanchez
 * Date: 27-Oct-2020
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Encoder.h>
#include <mac.h>
#include <hue.h>

EthernetClient client;

// Declare Bools
bool status;
bool HueOn;
bool lightState;
bool lastLight;
bool lastUltra;
bool roomOccupied;
bool roomNotOccupied;

// Declare Constants
const int echoPin=1; // attach digital pin Echo of HC-SR04
const int trigPin=0; //attach digital pin Trig of HC-SR04
const int nssPin=10; //attach pin nSS of Ethernet

const int hueWhite = 0xFFFFFF;

// Declare Variables 
int ultraState;   // variable for the distance measurement
int lightNum;
int HueColor;
int HueBright;
int encPosition;


void setup() {
  pinMode(nssPin, OUTPUT); //Sets the nssPin as an OUTPUT
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  
  Serial.begin(9600);
  delay(100);         // wait for Serial Monitor to Open 
  Serial.println("Starting my amazing Midterm Project Program");

//Start ethernet connection
  status = Ethernet.begin(mac);
  if (!status) {
    Serial.println("failed to configure Ethernet using DHCP");
    //no point in continueing 
    while(1);
    }
  //print your local IP address
  Serial.print("My IP address:");
  for (byte thisbyte = 0; thisbyte < 4; thisbyte++) {
    //print value of each byte of the IP address
    Serial.print(Ethernet.localIP()[thisbyte], DEC);
    if (thisbyte < 3) Serial.print(".");
    }
  Serial.println();

  HueColor=hueWhite;
  HueBright=188;
  lightNum=4;
}

void loop() {
  ultraState=ultra();
  if(ultraState!=lastUltra) {
    if(ultraState == true) {
      roomOccupied = !roomOccupied;
    }
    lastUltra=ultraState;
  }
  if(roomOccupied == true) {
    Serial.println("SOMEONE IS IN THE ROOM");
  }
  else {
    Serial.println("NO ONE IS IN THE ROOM");
  }  
}

bool ultra() {
  int duration;
  int distance;
  
  delay(1000); // Execute once per second 
  
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds, this is the pulse that will be detected
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  Serial.printf("Time %i, Distance: %i, Duration %i \n",millis(),distance, duration);
  if(distance<100) {
    return true;
  }
  else {
    return false;
  }
}

//void hue() {
//  lightState=digitalRead(nssPin);
//  if(lightState!=lastLight) {
//    if(lightState == HIGH) {
//      HueOn = !HueOn;
//    }
//    lastLight=lightState;
//  }
//  if(HueOn == HIGH) {
//      Serial.println("HueOn is TRUE");
//      setHue(lightNum,HueOn,HueColor,HueBright);
//  }
//  else {
//    Serial.println("HueOn is FALSE");
//    setHue(lightNum,HueOn,0,0);
//  }
//}
