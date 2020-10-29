/*
 * Project: Midterm Project
 * Description: Smart Room Controller
 * Author: Janel Sanchez
 * Date: 29-Oct-2020
*/

#include <SPI.h>
#include <Ethernet.h>
#include <Encoder.h>
#include <mac.h>
#include <hue.h>
#include <Adafruit_NeoPixel.h>
#include <colors.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include "Wemo.h"

EthernetClient client;
Adafruit_BME280 bme;

// Declare Bools
bool status;
bool lastUltra;
bool roomOccupied;

// Declare Constants
const int echoPin=1; // attach digital pin Echo of HC-SR04
const int trigPin=0; //attach digital pin Trig of HC-SR04
const int nssPin=10; //attach pin nSS of Ethernet
const int pixelPin=20; //attch digital pin DI of Pixel strip
const int pixelCount=60;
const int pinA=2;
const int pinB=3;
const int encGreen=23;

const int buttonPinBlue = 14;
const int ledPinBlue = 6;
const int buttonPinYellow = 15;
const int ledPinYellow = 7;
const int buttonPinRed = 16;
const int ledPinRed = 8;

Wemo wemo;

// Declare Variables 
int ultraState;   // variable for the distance measurement


Adafruit_NeoPixel pixel(pixelCount, pixelPin, NEO_GRB + NEO_KHZ800);
Encoder myEnc(pinA,pinB);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    22 // Reset pin # (or -1 if sharing Arduino reset pin)(Use pin not in use)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float tempC;
float tempF = 0;

void setup() {
  pinMode(nssPin, OUTPUT); //Sets the nssPin as an OUTPUT
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  pinMode(encGreen,OUTPUT);
  digitalWrite(encGreen,LOW);

  pinMode (buttonPinBlue,INPUT);
  pinMode (ledPinBlue, OUTPUT);
  pinMode (buttonPinYellow,INPUT);
  pinMode (ledPinYellow, OUTPUT);
  pinMode (buttonPinRed,INPUT);
  pinMode (ledPinRed, OUTPUT);
  
  Serial.begin(9600);
  delay(100);         // wait for Serial Monitor to Open 
  Serial.println("Starting my AMAZING Smart Room Controller Project Program");

//Start ethernet connection
  status = Ethernet.begin(mac);
  if (!status) {
    Serial.println("Failed to configure Ethernet using DHCP");
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

  myEnc.write(95);

  pixel.begin();
  pixel.show();

  status = bme.begin(0x76);
  if(!status) {
    Serial.println("Initialization Failed");
  }
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
}

void loop() {
  ultraState=ultra();
  if(ultraState!=lastUltra) {
    if(ultraState == true) {
      roomOccupied = !roomOccupied;
    }
    lastUltra=ultraState;
  }

  digitalWrite(encGreen,!roomOccupied);
  hue(roomOccupied);
  pixels(!roomOccupied);
  
  if(roomOccupied == true) {
    Serial.println("SOMEONE IS IN THE ROOM");
    displaybmevalues();    // display the BME values
  }
  else {
    Serial.println("NO ONE IS IN THE ROOM");
    display.clearDisplay();
    display.display();
  }
  tempC = bme.readTemperature();  
  tempF = (tempC*1.8)+32;

  buttons();
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
  int HueBright;
  HueBright=encBrightness();
  for(i=1;i<=5;i++) {
    setHue(i,hueState,HueYellow,HueBright);
    Serial.printf("HueOn is %i, For Bulb %i \n",hueState,i);
  }
}

void pixels(bool pixelState) {
  int j;
  int redPixel;
  int pix;
  int color;
  for(redPixel=0; redPixel<pixelCount; redPixel=redPixel+7) {
    for(j=0; j<7; j++) {
      pix = redPixel+j;
      color = pixelState*rainbow[j];
      pixel.setPixelColor(pix,color);
      pixel.setBrightness(15);
      pixel.show();
    }
  }
}

int encBrightness() {
  int encPosition;
  int encBright;
  
  encPosition=myEnc.read();
  if(encPosition>95) {
    myEnc.write(95);
  }
  if(encPosition<0) {
    myEnc.write(0);
  }
  encBright=map(encPosition,0,95,0,255);
  return encBright;
}

void displaybmevalues() {
  Serial.printf("Display to the Display \n");
  const char degreeSymbol=247;

  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.printf("Temp \n \n   %0.1f%cF \n", tempF,degreeSymbol);
  display.display();
}

void buttons() {
  static bool lastButtonBlue;
  static bool lastButtonYellow;
  static bool lastButtonRed;
  bool buttonStateBlue;
  bool buttonStateYellow;
  bool buttonStateRed;
  static bool wemoStateBlue;
  static bool wemoStateYellow;
  static bool wemoStateRed;  

// BLUE BUTTON
  buttonStateBlue=digitalRead(buttonPinBlue);
  if(buttonStateBlue!=lastButtonBlue) {
    if(buttonStateBlue == HIGH) {
      wemoStateBlue = !wemoStateBlue;
      digitalWrite(ledPinBlue,HIGH);
      Serial.printf("Blue Button/LED is ON \n");
    }
    lastButtonBlue=buttonStateBlue;
  }
  if(wemoStateBlue == HIGH) {
    wemo.switchON(3);
    Serial.printf("Wemo 3 is ON \n");
  }
  else {
      wemo.switchOFF(3);
      digitalWrite(ledPinBlue,LOW);
  }

// YELLOW BUTTON
  buttonStateYellow=digitalRead(buttonPinYellow);
  if(buttonStateYellow!=lastButtonYellow) {
    if(buttonStateYellow == HIGH) {
      wemoStateYellow = !wemoStateYellow;
      digitalWrite(ledPinYellow,HIGH);
      Serial.printf("Yellow Button/LED is ON \n");
    }
    lastButtonYellow=buttonStateYellow;
  }
  if(wemoStateYellow == HIGH) {
    wemo.switchON(2);
    Serial.printf("Wemo 2 is ON \n");
  }
  else {
      wemo.switchOFF(2);
      digitalWrite(ledPinYellow,LOW);
  }
  
// RED BUTTON
  buttonStateRed=digitalRead(buttonPinRed);
  if(buttonStateRed!=lastButtonRed) {
    if(buttonStateRed == HIGH) {
      wemoStateRed = !wemoStateRed;
      digitalWrite(ledPinRed,HIGH);
      Serial.printf("Red Button/LED is ON \n");
    }
    lastButtonRed=buttonStateRed;
  }
  if(wemoStateRed == HIGH) {
    wemo.switchON(1);
    Serial.printf("Wemo 1 is ON \n");
  }
  else {
      wemo.switchOFF(1);
      digitalWrite(ledPinRed,LOW);
  }
}
