/**
 * Object Detector with Vibrate Warning System for Electric Skateboard
 * Date: August 24th, 2020
 * Author: Anh Phan 
**/

//#include <LiquidCrystal.h> //Load Liquid Crystal Library
//LiquidCrystal LCD(11,10,9,2,3,4,5);  //Create Liquid Crystal Object called LCD

#define trigPin 13 //Sensor Echo pin connected to Arduino pin 13
#define echoPin 12 //Sensor Trip pin connected to Arduino pin 12
#define vibrator 8 //Vibration Motor connected to Arduino pin 7
#define redLED 2  //redLED connected to Arduino pin 2
#define greenLED 3  //greenLED connected to Arduino pin 3
#define blueLED 4 //blueLED connected to Arduino pin 4
const String warningLight[] = {"green", "yellow", "orange", "red"};
const int greenRGB_values[3] = {0, 255, 0};
const int yellowRGB_values[3] = {255, 255, 0};
const int orangeRGB_values[3] = {255, 69, 0};
const int redRGB_values[3] = {255, 0, 0};
boolean warningVibratorStatus = false;
String warningLightColor = "green";

void setup() 
{  
  // Starting Serial Terminal
  Serial.begin(9600); 
  
  //Define devices used
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(vibrator, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);

/*
  //LCD Display
  LCD.begin(16,2); //Tell Arduino to start 16 column 2 row LCD
  LCD.setCursor(0,0);  //Set LCD cursor to upper left corner, column 0, row 0
*/
}

void loop() 
{
  long duration, distance, distanceCm, distanceInch;
  int delayTime = 100;
  const int safeDistanceCm = 300;
  const int safeDistanceIn = 118;
  const int moderateDistanceCm = 200;
  const int moderateDistanceIn = 79;
  const int dangerDistanceCm = 100;
  const int dangerDistanceIn = 39;

  //Duration count from the moment the Ultrasonic sensor starts to the moment it recieves the signal back
  duration = getDuration();
  //Calculate the distance (apply the speed of sound)
  distanceCm = inCentimeter(duration);    
  distanceInch = inInch(duration);

  //Update the status of the Vibration Motor
  warningSystem(safeDistanceCm, safeDistanceIn, moderateDistanceCm, moderateDistanceIn, dangerDistanceCm, dangerDistanceIn, distanceCm, distanceInch);
  
/*
  //Display on LCD screen
  printDistanceInCentimeterLCD(distanceCm);
  printDistanceInInchLCD(distanceInch);
*/

  //Update system on Serial port for debugging
  printSerialReport(distanceCm, distanceInch, warningVibratorStatus, warningLightColor);
  
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

/*
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
*/

//Activate the Vibrate Motor if below the safe range
void warningSystem(int safeDistanceCm, int safeDistanceIn, int moderateDistanceCm, int moderateDistanceIn, int dangerDistanceCm, int dangerDistanceIn, long distanceCm, long distanceInch)
{
  //First zone (Distance: 2m to 3m away)
  if ((distanceCm < safeDistanceCm && distanceCm > moderateDistanceCm) || (distanceInch < safeDistanceIn && distanceInch > moderateDistanceIn)) 
  {
    firstVibrationZone();
    lightActivator(yellowRGB_values[0], yellowRGB_values[1], yellowRGB_values[2]);
    serialReport(true, 1);
  }
  //Second zone (Distance: 1m to 2m away)
  else if ((distanceCm < moderateDistanceCm && distanceCm > dangerDistanceCm) || (distanceInch < moderateDistanceIn && distanceCm > dangerDistanceCm)) 
  {
    secondVibrationZone();
    lightActivator(orangeRGB_values[0], orangeRGB_values[1], orangeRGB_values[2]);
    serialReport(true, 2);
  } 
  //Third zone (Distance: within 1m)
  else if (distanceCm < dangerDistanceCm || distanceInch < dangerDistanceIn) 
  {
    thirdVibrationZone();
    lightActivator(redRGB_values[0], redRGB_values[1], redRGB_values[2]);
    serialReport(true, 3);
  } 
  //Nothing detected
  else 
  {
    safeVibrationZone();
    lightActivator(greenRGB_values[0], greenRGB_values[1], greenRGB_values[2]);
    serialReport(false, 0);
  }
}

//Four vibration stages
void safeVibrationZone()
{
  digitalWrite(vibrator, LOW);
}

void firstVibrationZone()
{
  digitalWrite(vibrator, HIGH);
  delay(500);
  digitalWrite(vibrator, LOW);
}

void secondVibrationZone()
{
  digitalWrite(vibrator, HIGH);
  delay(250);
  digitalWrite(vibrator, LOW);
}

void thirdVibrationZone()
{
  digitalWrite(vibrator, HIGH);
}

//Activate RGB LED light
void lightActivator(int redRGB, int greenRGB, int blueRGB)
{
  analogWrite(redLED, redRGB);
  analogWrite(greenLED, greenRGB);
  analogWrite(blueLED, blueRGB);
}

//Report warning system status
void serialReport(boolean warningVibratorStatus, int warningLightValue)
{
  warningVibratorStatus = warningVibratorStatus;
  warningLightColor = warningLight[warningLightValue];
}

//Update system into Serial port
void printSerialReport(long distanceCm, long distanceIn, boolean warningVibratorStatus, String warningLightColor)
{
  Serial.println("Object Distance & Vibration & Light");
  Serial.print(distanceCm);
  Serial.println(" cm");
  Serial.print(distanceIn);
  Serial.println(" in");
  Serial.println("Vibration: ");
  if (warningVibratorStatus)
  {
    Serial.println("true");
  } else {
    Serial.println("false");
  }
  Serial.print("Warning Color: ");
  Serial.println(warningLightColor);
  Serial.println(" ");
}

