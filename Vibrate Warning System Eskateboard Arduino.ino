/**
 * Object Detector with Vibrate Warning System for Electric Skateboard
 * Date: August 24th, 2020 - August 30th, 2020
 * Author: Anh Phan 
**/

//#include <LiquidCrystal.h> //Load Liquid Crystal Library
//LiquidCrystal LCD(11,10,9,2,3,4,5);  //Create Liquid Crystal Object called LCD

#define trigPin_RF 7 //Sensor Echo Left Front pin connected to Arduino pin 7
#define echoPin_RF 6 //Sensor Trip Left Front pin connected to Arduino pin 6
#define echoPin_F 5 //Sensor Trip Front pin connected to Arduino pin 5
#define trigPin_F 4 //Sensor Echo Front pin connected to Arduino pin 4
#define echoPin_LF 3 //Sensor Trip Right Front pin connected to Arduino pin 3
#define trigPin_LF 2 //Sensor Echo Right Front pin connected to Arduino pin 2
#define vibrator 8 //Vibration Motor connected to Arduino pin 8
#define redLED 11  //redLED connected to Arduino pin 11
#define greenLED 10  //greenLED connected to Arduino pin 10
#define blueLED 9 //blueLED connected to Arduino pin 9
#define beeper 12 //beeper connected to Arduino pin 12
#define button 13 //button connected to Arduino pin 13
const String warningLight[] = {"green", "yellow", "orange", "red"};
const int greenRGB_values[3] = {0, 255, 0};
const int yellowRGB_values[3] = {255, 255, 0};
const int orangeRGB_values[3] = {255, 69, 0};
const int redRGB_values[3] = {255, 0, 0};
const String booleanDef[2] = {"false", "true"};
String warningVibratorStatus;
String beeperStatus;
String currentSwitchStatus = booleanDef[0];
String warningLightColor = "green";

void setup() 
{  
  // Starting Serial Terminal
  Serial.begin(9600); 
  
  //Define devices used
  pinMode(trigPin_LF, OUTPUT);
  pinMode(echoPin_LF, INPUT);
  pinMode(trigPin_F, OUTPUT);
  pinMode(echoPin_F, INPUT);
  pinMode(trigPin_RF, OUTPUT);
  pinMode(echoPin_RF, INPUT);
  pinMode(vibrator, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(beeper, OUTPUT);
  pinMode(button, INPUT_PULLUP);

/*
  //LCD Display
  LCD.begin(16,2); //Tell Arduino to start 16 column 2 row LCD
  LCD.setCursor(0,0);  //Set LCD cursor to upper left corner, column 0, row 0
*/
}

void loop() 
{
  long distanceCm_LF, distanceCm_F, distanceCm_RF, shortDistance;
  int delayTime = 100;
  const int safeDistanceCm = 400;
  const int moderateDistanceCm = 300;
  const int dangerDistanceCm = 200;

  //Save the distance (apply the speed of sound)
  distanceCm_LF = getDistanceLF();
  distanceCm_F = getDistanceF();
  distanceCm_RF = getDistanceRF();

  //Find the smallest distance
  shortDistance = isSmallestDistance(distanceCm_LF, distanceCm_F, distanceCm_RF);

  //Update the status of the Vibration Motor
  warningSystem(safeDistanceCm, moderateDistanceCm, dangerDistanceCm, shortDistance);
  
/*
  //Display on LCD screen
  printDistanceInCentimeterLCD(distanceCm);
  printDistanceInInchLCD(distanceInch);
*/

  //Update system on Serial port for debugging
  printSerialReport(distanceCm_LF, distanceCm_F, distanceCm_RF, shortDistance, warningVibratorStatus, beeperStatus, warningLightColor);
  
  delay(delayTime);
}

//Ping each sensor
long pingLF()
{
  digitalWrite(trigPin_LF, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_LF, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_LF, LOW);
  
  return pulseIn(echoPin_LF, HIGH);
}

long pingF()
{
  digitalWrite(trigPin_F, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_F, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_F, LOW);
  
  return pulseIn(echoPin_F, HIGH);
}

long pingRF()
{
  digitalWrite(trigPin_RF, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_RF, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_RF, LOW);
  
  return pulseIn(echoPin_RF, HIGH);
}

//Convert into distance (cm)
long inCentimeter(long duration) {
   return duration / 29 / 2;
}
/*
//Convert into distance (in)
long inInch(long duration) {
   return duration / 74 / 2;
}
*/

//Calculate distance from sensor to object
long getDistanceLF()
{
  return inCentimeter(pingLF());
}

long getDistanceF()
{
  return inCentimeter(pingF());
}

long getDistanceRF()
{
  return inCentimeter(pingRF());
}

//Identify the closest distance
long isSmallestDistance(long distanceCm_LF, long distanceCm_F, long distanceCm_RF)
{
  int sortQuantity = 3;
  int cycle = 0;
  int sorted = 0;
  long distances[sortQuantity] = {distanceCm_LF, distanceCm_F, distanceCm_RF};
  
  while ((cycle-sorted) < sortQuantity)
  {
    for (int i = 1; i <= sortQuantity; i++)
    {
      long tempDistance = 0;
      if (distances[i-1] > distances[i])
      {
        tempDistance = distances[i-1];
        distances[i-1] = distances[i];
        distances[i] = tempDistance;
        sorted++;
      }
      cycle++;
    }
  }

  return distances[0];
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
void warningSystem(int safeDistanceCm, int moderateDistanceCm, int dangerDistanceCm, long distanceCm)
{
  //First zone (Distance: 3m to 4m away)
  if (distanceCm < safeDistanceCm && distanceCm > moderateDistanceCm) 
  {
    firstVibrationZone();
    firstBuzzZone();
    lightActivator(yellowRGB_values[0], yellowRGB_values[1], yellowRGB_values[2]);
    serialReport(booleanDef[1], booleanDef[1], 1);
  }
  //Second zone (Distance: 2m to 3m away)
  else if (distanceCm < moderateDistanceCm && distanceCm > dangerDistanceCm) 
  {
    secondVibrationZone();
    secondBuzzZone();
    lightActivator(orangeRGB_values[0], orangeRGB_values[1], orangeRGB_values[2]);
    serialReport(booleanDef[1], booleanDef[1], 2);
  } 
  //Third zone (Distance: within 2m)
  else if (distanceCm < dangerDistanceCm) 
  {
    thirdVibrationZone();
    thirdBuzzZone();
    lightActivator(redRGB_values[0], redRGB_values[1], redRGB_values[2]);
    serialReport(booleanDef[1], booleanDef[1], 3);
  } 
  //Nothing detected
  else 
  {
    safeVibrationZone();
    safeBuzzZone();
    lightActivator(greenRGB_values[0], greenRGB_values[1], greenRGB_values[2]);
    serialReport(booleanDef[0], booleanDef[0], 0);
  }
}

//Four Vibration Stages
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

//Activate Beeper
void beeperActivator(int freq, int onDuration)
{
  if (switchStatus() == booleanDef[1])
  {
    tone(beeper, freq, onDuration);
  } else {
    noTone(beeper);
  }
}

//Four Buzz Stages
void safeBuzzZone()
{
  noTone(beeper);
}

void firstBuzzZone()
{
  beeperActivator(5000, 500);
}

void secondBuzzZone()
{
  beeperActivator(5000, 200);
}

void thirdBuzzZone()
{
  beeperActivator(5000, 1000);
}

//Check switch status (push to turn on/off)
String switchStatus()
{
  int buttonStatus = digitalRead(button);
  String switchStatus = currentSwitchStatus;

  if (buttonStatus == 0 && currentSwitchStatus == booleanDef[1])
  {
    switchStatus = booleanDef[0];
  } 
  else if (buttonStatus == 0 && currentSwitchStatus == booleanDef[0])
  {
    switchStatus = booleanDef[1];
  } 
  currentSwitchStatus = switchStatus;

  return switchStatus;
}

//Report warning system status
void serialReport(String VibratorStatus, String buzzStatus, int warningLightValue)
{
  beeperStatus = buzzStatus;
  warningVibratorStatus = VibratorStatus;
  warningLightColor = warningLight[warningLightValue];
}

//Update system into Serial port
void printSerialReport(long distanceCm_LF, long distanceCm_F, long distanceCm_RF, long shortDistance, String vibratorStatus, String beeperStatus, String warningLightColor)
{
  Serial.println("Object Distance & Vibration & Light");

  Serial.print("LF: ");
  Serial.print(distanceCm_LF);
  Serial.println(" cm");

  Serial.print("F: ");
  Serial.print(distanceCm_F);
  Serial.println(" cm");

  Serial.print("RF: ");
  Serial.print(distanceCm_RF);
  Serial.println(" cm");

  Serial.print("Shortest Distance: ");
  Serial.print(shortDistance);
  Serial.println("cm");

  Serial.print("Vibration: ");
  Serial.println(vibratorStatus);

  Serial.print("Beeper (only work if button is true): ");
  Serial.println(beeperStatus);

  Serial.print("Button: ");
  Serial.println(switchStatus());

  Serial.print("Warning Color: ");
  Serial.println(warningLightColor);
  Serial.println(" ");
}