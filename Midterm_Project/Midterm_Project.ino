/*
 * Project: Midterm Project
 * Description: Smart Room Controller
 * Author: Janel Sanchez
 * Date: 27-Oct-2020
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Ethernet.h>
#include <mac.h>
#include <Encoder.h>
#include <wemo.h>
#include <Adafruit_NeoPixel.h>
#include <colors.h>

Adafruit_BME280 bme;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     9 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const char degreeSymbol = 247;

float tempC;
float tempF = 0;

const int pinA = 3;
const int pinB = 2;
const int encButton = 23;

const int pinPixelStrip = 20;
const int pixelCount = 60;

const int buttonPinBlue = 14;
bool buttonStateBlue = 0;
const int ledPinBlue = 6;

const int buttonPinYellow = 15;
bool buttonStateYellow = 0;
const int ledPinYellow = 7;

const int buttonPinRed = 16;
bool buttonStateRed = 0;
const int ledPinRed = 8;

Encoder myEnc(3,2);
Adafruit_NeoPixel pixel (pixelCount,pinPixelStrip, NEO_GRB + NEO_KHZ800);

int encPos;
int pos;
int pixelPos;

EthernetClient client;
bool status;
bool lastButtonBlue;
bool lastButtonYellow;
bool lastButtonRed;
bool wemoState;

Wemo wemo;

void setup() {
  
  pinMode(10, OUTPUT);      //ethernet+wemo setup begin
  digitalWrite(10, HIGH);

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  //Open Serial Communication and wait for port to open:
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Starting Program");

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
  Serial.println();         //ethernet+wemo setup end

  pinMode (buttonPinBlue,INPUT);    //button+LED setup begin
  pinMode (ledPinBlue, OUTPUT);
  pinMode (buttonPinYellow,INPUT);
  pinMode (ledPinYellow, OUTPUT);
  pinMode (buttonPinRed,INPUT);
  pinMode (ledPinRed, OUTPUT);      //button+LED setup end

  pixel.begin();                    //Pixel setup begin
  pixel.show();
  pixel.setBrightness(88);          //Pixel setup end

  status = bme.begin(0x76);           //BME setup begin
  if(!status) {
    Serial.println("Initialization Failed");
  }                                   //BME setup end
}

void loop(){
  
}


void threebutton() {
  // put your main code here, to run repeatedly:
  buttonStateBlue=digitalRead(buttonPinBlue);     //Button+Wemo Loop Start
  if(buttonStateBlue!=lastButtonBlue) {
    if(buttonStateBlue == HIGH) {
      wemoState = !wemoState;
    }
    lastButtonBlue=buttonStateBlue;
  }
  if(wemoState == HIGH) {
    wemo.switchON(0);
  }
  if(wemoState == LOW) {
    wemo.switchOFF(0);  
  }

  buttonStateYellow=digitalRead(buttonPinYellow);
  if(buttonStateYellow!=lastButtonYellow) {
    if(buttonStateYellow == HIGH) {
      wemoState = !wemoState;
    }
    lastButtonYellow=buttonStateYellow;
  }
  if(wemoState == HIGH) {
    wemo.switchON(1);
  }
  if(wemoState == LOW) {
    wemo.switchOFF(1);  
  }
  
  buttonStateRed=digitalRead(buttonPinRed);
  if(buttonStateRed!=lastButtonRed) {
    if(buttonStateRed == HIGH) {
      wemoState = !wemoState;
    }
    lastButtonRed=buttonStateRed;
  }
  if(wemoState == HIGH) {
    wemo.switchON(2);
  }
  if(wemoState == LOW) {
    wemo.switchOFF(2);  
  }                                                 //Button+Wemo Loop End

}

void temp() {
  tempC = bme.readTemperature();
  pressPA = ((bme.readPressure()/100.0)*0.03);
  humidRH = bme.readHumidity();
  
  tempF = (tempC*1.8)+32;

  Serial.printf("Temp %0.1f°F \n", tempF);
  delay(50000);
}
