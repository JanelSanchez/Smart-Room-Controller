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
#include <Adafruit_NeoPixel.h>
#include <colors.h>

EthernetClient client;

// Declare Bools
bool status;
bool HueOn;
bool lightState;
bool lastLight;
bool lastUltra;
bool roomOccupied;

// Declare Constants
const int echoPin=1; // attach digital pin Echo of HC-SR04
const int trigPin=0; //attach digital pin Trig of HC-SR04
const int nssPin=10; //attach pin nSS of Ethernet
const int pixelPin=20; //attch digital pin DI of Pixel strip
const int pixelCount=60;

// Declare Variables 
int ultraState;   // variable for the distance measurement
int lightNum;
int HueColor;
int HueBright;
int encPosition;

Adafruit_NeoPixel pixel(pixelCount, pixelPin, NEO_GRB + NEO_KHZ800);


void setup() {
  pinMode(nssPin, OUTPUT); //Sets the nssPin as an OUTPUT
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  
  Serial.begin(9600);
  delay(100);         // wait for Serial Monitor to Open 
  Serial.println("Starting my AMAZING Smart Room Controller Project Program");

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

  HueBright=188;

//  pixel.begin();
//  pixel.show();
}

void loop() {
  ultraState=ultra();
  if(ultraState!=lastUltra) {
    if(ultraState == true) {
      roomOccupied = !roomOccupied;
    }
    lastUltra=ultraState;
  }
  hue(roomOccupied);
  if(roomOccupied == true) {
    Serial.println("SOMEONE IS IN THE ROOM");
  }
  else {
    Serial.println("NO ONE IS IN THE ROOM");
//    pixel();
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

void hue(bool hueState) {
  int i;
  for(i=1;i<=5;i++) {
    setHue(i,hueState,HueYellow,HueBright);
    Serial.printf("HueOn is %i, For Bulb %i \n",hueState,i);
  }
}

//void pixels() {
//  int pixel;
//  for (pixel=0; pixel<0; i++) {
//    pixel.setPixelColor(pixel,0xBBBB);
//    pixel.setBrightness(25);
//    pixel.show();
//    delay(250);
//    pixel.clear();
//  }  
//}
