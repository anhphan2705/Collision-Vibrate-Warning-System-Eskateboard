/**
 * Object Detector with Vibrate Warning System for Electric Skateboard
 * Date: August 24th, 2020
 * Author: Anh Phan 
**/

#include <LiquidCrystal.h> //Load Liquid Crystal Library
LiquidCrystal LCD(11,10,9,2,3,4,5);  //Create Liquid Crystal Object called LCD

#define trigPin 13 //Sensor Echo pin connected to Arduino pin 13
#define echoPin 12 //Sensor Trip pin connected to Arduino pin 12
#define vibrator 8 //Vibration Motor connected to Arduino pin 8

void setup() 
{  
  // Starting Serial Terminal
  Serial.begin(9600); 
  
  //Define devices used
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(vibrator, OUTPUT);
  
  //LCD Display
  LCD.begin(16,2); //Tell Arduino to start 16 column 2 row LCD
  LCD.setCursor(0,0);  //Set LCD cursor to upper left corner, column 0, row 0
}

void loop() 
{
  long duration, distance, distanceCm, distanceInch;
  int delayTime = 100;
  int safeDistanceCm = 300;
  int safeDistanceIn = 118;
  boolean vibratorStatus = false;

  //Duration count from the moment the Ultrasonic sensor starts to the moment it recieves the signal back
  duration = getDuration();
  //Calculate the distance (apply the speed of sound)
  distanceCm = inCentimeter(duration);    
  distanceInch = inInch(duration);

  //Update the status of the Vibration Motor
  vibratorStatus = vibrateActivator(safeDistanceCm, safeDistanceIn, distanceCm, distanceInch);

  //Display on LCD screen
  printDistanceInCentimeterLCD(distanceCm);
  printDistanceInInchLCD(distanceInch);

  //Update system on Serial port for debugging
  printSerial(distanceCm, distanceInch, vibratorStatus);
  
  delay(delayTime);
}

//Calculate total duration of the sonar bounce
long getDuration()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  return pulseIn(echoPin, HIGH);
}

//Convert into distance (cm)
long inCentimeter(long duration) {
   return duration / 29 / 2;
}
//Convert into distance (in)
long inInch(long duration) {
   return duration / 74 / 2;
}

//LCD Print
void printDistanceInCentimeterLCD(long distanceCm)
{
  LCD.setCursor(0,0);  
  LCD.print("                  ");
  LCD.setCursor(0,0);  
  LCD.print("Range (cm): ");
  LCD.print(distanceCm); 
} 

void printDistanceInInchLCD(long distanceInch)
{
  LCD.setCursor(0,1);
  LCD.print("                  ");
  LCD.setCursor(0,1);
  LCD.print("Range (in): ");
  LCD.print(distanceInch);
} 

//Activate the Vibrate Motor if below the safe range
boolean vibrateActivator(int safeDistanceCm, int safeDistanceIn, long distanceCm, long distanceInch)
{
    if (distanceCm < safeDistanceCm || distanceInch < safeDistanceIn) 
  {
    digitalWrite(vibrator, HIGH);
    return true;
  } else {
    digitalWrite(vibrator, LOW);
    return false;
  }
}

//Update system into Serial port
void printSerial(long distanceCm, long distanceIn, boolean vibratorStatus)
{
  Serial.println("Object Distance & Vibration");
  Serial.print(distanceCm);
  Serial.println(" cm");
  Serial.print(distanceIn);
  Serial.println(" in");
  Serial.println(vibratorStatus);
}