#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <SD.h>
//#include "SdFat.h"
//SdFat SD;

// Pin connections
#define ADS1115_ADDRESS 0x48
#define RTC_SDA_PIN 21
#define RTC_SCL_PIN 22
#define LCD_ADDRESS 0x27
#define SD_CS_PIN 5
//indicator connections
#define LedR 25
#define LedG 32                               
#define LedB 33
#define Buzz 14
#define ToggleButton1 26
#define ToggleButton2 27  
#define limitswitch 2

String mode1 = "R80";
String mode2 = "R81";
String mode3 = "R82";
String mode4 = "R83";
String mode5 = "R84";
int m1low = 1;
int m1high = 2;
int m2low = 3;
int m2high = 4;
int m3low = 5;
int m3high = 6;
int m4low = 7;
int m4high = 8;
int m5low = 9;
int m5high = 10;

// Create instances
Adafruit_ADS1115 ads;
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);
RTC_DS1307 rtc;
File dataFile;

  int slno = 0;
  int n = 0;

void setup() {
                                                                // wifi browser 
  Serial.begin(9600);                                                         
  
  Wire.begin();

  pinMode(LedB, OUTPUT);
  pinMode(LedR, OUTPUT);
  pinMode(LedG, OUTPUT);
  pinMode(ToggleButton1, INPUT);
  pinMode(Buzz, INPUT);
  pinMode(limitswitch, INPUT);

  digitalWrite(LedB, HIGH);
  digitalWrite(LedR, HIGH);
  digitalWrite(LedG, HIGH);

  // Initialize LCD module
  lcd.init();
  lcd.backlight();

  // Initialize ADS1115 ADC module
    if (!ads.begin(ADS1115_ADDRESS)) {                        
    Serial.println("Failed to initialize ADS.");             // check if the led stays on, wile will cause to be in if indefinitely?
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" ADS Failed!");
    digitalWrite(LedB, LOW);
    while (1);                                             
  }
   ads.setGain(GAIN_FOUR);      // 4x gain   +/- 1.024V  1 bit =  0.03125mV

  // Initialize RTC module
  //rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  rtc.begin();
  if(!rtc.begin()) {
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(" RTC Failed");
   delay(5000);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Failed to initialize SD card!"); 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" SD Failed!");
    digitalWrite(LedB, LOW);                   
    delay(5000);
  }
  Serial.println("SD card initialized.");

  // Create a file with current date and time as the filename
  String fileName = getFileName();
  dataFile = SD.open(fileName, FILE_WRITE);

  if(!dataFile){
    Serial.println("Error creating file!");  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" File Error");
    digitalWrite(LedB, LOW);                 
    delay(5000);
  }
  Serial.println("created file ");
  Serial.print(fileName);
  delay(100);                                                    
  dataFile.println("Sl.No.,Time,Resistance(ohm)");  
  dataFile.flush();                         
  dataFile.close();

String time = getTime();                                     
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("HITECH MAGNETICS");
    delay(5000);
    lcd.clear();  
    lcd.setCursor(0, 0);
    lcd.print(fileName);
    lcd.setCursor(1, 1);
    lcd.print("  ");
    lcd.print(time);
delay(5000);
}

void loop() {
  
  // Read voltage from ADS1115 // pending to set gain 
  int16_t adc0 = ads.readADC_SingleEnded(0);
  float voltage = ads.computeVolts(adc0);  // Convert ADC value to voltage
  float resistance = getresistance(voltage);

  // Get current date and time from RTC module
  DateTime now = rtc.now();

  // Print voltage on LCD
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(resistance);                                // add modes i.e R81,R84 etc
  lcd.print(" ohm");

  slno += 1;

  // Print voltage, date, and time on Serial Monitor
  Serial.print("Resistance: ");
  Serial.print(resistance);
  Serial.print(" Voltage: ");
  Serial.print(voltage);  
  Serial.print("V\tDate: ");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.day(), DEC);
  Serial.print("/");
  Serial.print(now.year(), DEC);
  Serial.print("\tTime: ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.println(now.second(), DEC);

//to toggle between modes
int temp = n;
if( digitalRead(ToggleButton1) == HIGH ){
  n += 1;
  delay(250);
  slno = 0;  
  if(n>5)
  n = 0;
}

if(n==0){
  // Save voltage, date, and time to SD card
 if( resistance > m1low && resistance > m1high ) {
   digitalWrite(LedG, LOW);
   digitalWrite(Buzz, HIGH);
   delay(250);
 }
  dataFile = SD.open(getFileName(), FILE_WRITE);             //check 
  if (dataFile /*&& limitswitch==HIGH*/) {
    if(temp!=n){
     dataFile.println(mode1);}
    dataFile.print(slno);
    dataFile.print(",");
    dataFile.print(now.hour(), DEC);
    dataFile.print(":");
    dataFile.print(now.minute(), DEC);
    dataFile.print(":"); 
    dataFile.print(now.second(), DEC);
    dataFile.print(",");
    dataFile.println(resistance);
    dataFile.close();
    delay(100);
    Serial.println("Data saved to SD card.");
  } else {
    Serial.println("Error opening file!");                             //add error indicator            
  }
 }

if(n==1){
  // Save voltage, date, and time to SD card
 if( resistance > m2low && resistance > m2high ) {
   digitalWrite(LedG, LOW);
   digitalWrite(Buzz, HIGH);
   delay(250);
 }
  dataFile = SD.open(getFileName(), FILE_WRITE);             //check 
  if (dataFile /*&& limitswitch==HIGH*/) {
    if(temp!=n){
     dataFile.println(mode2);}
    dataFile.print(slno);
    dataFile.print(",");
    dataFile.print(now.hour(), DEC);
    dataFile.print(":");
    dataFile.print(now.minute(), DEC);
    dataFile.print(":"); 
    dataFile.print(now.second(), DEC);
    dataFile.print(",");
    dataFile.println(resistance);
    dataFile.close();
    delay(100);
    Serial.println("Data saved to SD card.");
  } else {
    Serial.println("Error opening file!");                             //add error indicator            
  }
 }

}

float getresistance(float voltage) {
 
  float Vin = 3.3;
  float Vout = voltage;
  float R1 = 30000.0;
  float R2 = 0.0;
  float buffer = 0.0;
   
    R2 = (Vout*R1)/(Vin-Vout);
    Serial.print("Vout: ");
    Serial.println(Vout);
    Serial.print("R2: ");
    Serial.println(R2);
    return R2;

}

String getFileName() {
  DateTime now = rtc.now();
  String fileName = "/";
  fileName += now.year();
  fileName += "-";
  if (now.month() < 10) fileName += "0";
  fileName += now.month();
  fileName += "-";
  if (now.day() < 10) fileName += "0";
  fileName += now.day();

  fileName += ".txt";
  return fileName;
}

String getTime() {
  DateTime now = rtc.now();
   String time = ".";
    time += now.hour();
    time += ":";
    time += now.minute();
    time += ".";
  return time;
}


