#include <Servo.h> 
 
Servo leftServo;
Servo rightServo;
Servo atlasServo;

int i = 0;
int x = 0;
long left = 0;
long right = 0;
int hasSwitched = 0;

int leftAvgHelper = 0;
int rightAvgHelper = 0;
int leftIndex = 0;
int rightIndex = 0;

long frontDist = 0;
long leftDist = 0;
long rightDist = 0;
long leftBright = 0;
long rightBright = 0;

const int leftPhotoPin = 3;
const int rightPhotoPin = 4;
const int topPhotoPin = 5;
const int arrSize = 5;
int turnsLeft = 0;
int turnsRight = 0;

const int leftIRPin = 6;
const int rightIRPin = 7;

const int leftPingPin = 8;
const int rightPingPin = 9;
const int leftServoPin = 10;

const int rightServoPin = 11;
const int atlasServoPin = 12;
const int atlasPingPin = 13;

long topPhotoAvg = 0;

void setup() {
  leftServo.attach(leftServoPin);
  rightServo.attach(rightServoPin);
  atlasServo.attach(atlasServoPin);  
  atlasServo.write(85);

  pinMode(leftIRPin, INPUT);
  delay(1000);
  for (i = 0; i < 100; i = i + 1){
    topPhotoAvg += analogRead(topPhotoPin);
    delay(40);
  }

  topPhotoAvg = topPhotoAvg / 100;
  Serial.begin(9600);
}

void loop() {
  Serial.print(topPhotoAvg);
  Serial.print("\n");
  Serial.print(analogRead(topPhotoPin));
  Serial.print("\n");
  if (hasSwitched == 2){
    Serial.print("going in room\n");
    if(topPhotoPin >= topPhotoAvg -100);
    hasSwitched = 0;
    usePhoto();
  }
  else if(hasSwitched == 1){
    Serial.print("using ping 2\n");
    for (int i = 0; i != 10; i++){
      usePing();
    }
    hasSwitched = 2;
    Serial.print("DONE\n");
  }
  else{
    Serial.print("using ping\n");
    usePing();
    Serial.print(turnsRight);
    Serial.print(turnsLeft);
    if (analogRead(topPhotoPin) <= topPhotoAvg-200){
      hasSwitched = 1;
    }
  }
 }

void usePhoto(void){
  int leftPhoto = analogRead(leftPhotoPin);
  int rightPhoto = analogRead(rightPhotoPin);
  Serial.print("left");
  Serial.print('\t');
  Serial.print("top");
  Serial.print('\t');
  Serial.print("right");
  Serial.print('\n');
  Serial.print(leftPhoto);
  Serial.print('\t');
  Serial.print(analogRead(topPhotoPin));
  Serial.print('\t');
  Serial.print(rightPhoto);
  Serial.print('\n');

  if(abs(leftPhoto - rightPhoto) < 50){
    Serial.print("going straight\n");
    leftServo.writeMicroseconds(1400);
    rightServo.writeMicroseconds(1400);
  }else if (leftPhoto > rightPhoto){
    Serial.print("going left\n");
    leftServo.writeMicroseconds(1450);
    rightServo.writeMicroseconds(1400);
  }

  else {
    Serial.print("going right\n");
    leftServo.writeMicroseconds(1400);
    rightServo.writeMicroseconds(1450);
  }
  delay(200);
}

void usePing(void){
   leftAvgHelper = 0;
  rightAvgHelper = 0;
  
  if (x == 1){
     atlasServo.write(135);
  }
  if (x == 3){
     atlasServo.write(35);
     x = -1;
  }

  frontDist = pingSensor(atlasPingPin);
  leftDist = pingSensor(leftPingPin);
  rightDist = pingSensor(rightPingPin);
  Serial.print(turnsLeft);
  Serial.print("\t");
  Serial.print(turnsRight);
  Serial.print("\n");
  //if there is room in front
  if (frontDist > 40) {
    //but no room to the left
    if (leftDist < 8) {
      //turn right
      leftServo.writeMicroseconds(1400);
      rightServo.writeMicroseconds(1650);
      Serial.print("turning right\n");
      if (turnsLeft > 0){
        turnsLeft -= 1;
      }
      else {
        turnsRight += 1;
      }
      
    }//if no room to the right
    else if (rightDist < 8) {
      //turn left
      leftServo.writeMicroseconds(1650);
      rightServo.writeMicroseconds(1400);
      Serial.print("turning left\n");
      if (turnsRight > 0){
        turnsRight -= 1;
      }
      else {
        turnsLeft += 1;
      }
    }//if room both ways but it has been going right turn left
    else if (turnsRight > 0){
      leftServo.writeMicroseconds(1650);
      rightServo.writeMicroseconds(1400);
      Serial.print("Room, correcting left\n");
      turnsRight -= 1;
    } 
    //if room both ways but it has been going left turn right
    else if (turnsLeft > 0){
      leftServo.writeMicroseconds(1400);
      rightServo.writeMicroseconds(1650);
      Serial.print("Room, correcting right\n");
      turnsLeft -= 1;
    }
    //else go straight
    else{
      leftServo.writeMicroseconds(1400);
      rightServo.writeMicroseconds(1400);
      Serial.print("Going straight\n");
    }
  }
    //if theres no room in front but room to the left
  else if (leftDist > 50){
    leftServo.writeMicroseconds(1650);
    rightServo.writeMicroseconds(1400);
    Serial.print("No room forward, going left\n");
    if (turnsRight > 0){
        turnsRight -= 1;
      }
      else {
        turnsLeft += 1;
      }
      //if no room in front but room to the right
  }
  else if (rightDist > 50){
    leftServo.writeMicroseconds(1400);
    rightServo.writeMicroseconds(1650);
    Serial.print("No room forward going right\n");
    if (turnsLeft > 0){
        turnsLeft -= 1;
      }
      else {
        turnsRight += 1;
      }
  }
  //go backwards
  else{
    Serial.print("No room anywhere. Going backwards\n");
    leftServo.writeMicroseconds(1650);
    rightServo.writeMicroseconds(1650);
  }

  x = x + 1;
  delay(200);
}

long pingSensor(int pin){
  long duration;

  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);

  pinMode(pin, INPUT);
  duration = pulseIn(pin, HIGH);
  
  return duration / 29 / 2;
}
