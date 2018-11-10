/*************************************************** 
  Written by John Connolly
  Coupling 3 servo robot arm with arduino electric gyro.
  started 4/28/2017
  Testing phase
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

#define SERVOMIN  115 // this is the 'minimum' pulse length count
#define SERVOMAX  550 // this is the 'maximum' pulse length count
#define SERVOMID 300
#define SERVOMOVE 5
#define XBOUND 0.1
#define YBOUND 0.1
#define ZBOUND 0.11

//pwm.setPWM(0, 0, servo1PWM);

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

int servo0PWM = SERVOMID;
int servo1PWM = SERVOMID;
int servo2PWM = SERVOMID;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  setupMPU();

  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
  
  pwm.setPWM(0, 0, SERVOMID);
  pwm.setPWM(1, 0, SERVOMID);
  pwm.setPWM(2, 0, SERVOMID);
  
  delay(10000);
}


void loop() {
  recordAccelRegisters();
  
  //Y direction movement, servo 0
  if(gForceY > YBOUND){
    servo0PWM = servo0PWM - SERVOMOVE;
    pwm.setPWM(0, 0, servo0PWM);
  }
  else if(gForceY < -(YBOUND)){
    servo0PWM = servo0PWM + SERVOMOVE;
    pwm.setPWM(0, 0, servo0PWM);
  }

  //x direction movement, servo 1
  if(gForceX > XBOUND){
    servo1PWM = servo1PWM + SERVOMOVE;
    pwm.setPWM(1, 0, servo1PWM);
  }
  else if(gForceX < -(XBOUND)){
    servo1PWM = servo1PWM - SERVOMOVE;
    pwm.setPWM(1, 0, servo1PWM);
  }
/*
  //z direction movement, servo 2
  if(gForceZ > ZBOUND){
    servo1PWM = servo1PWM + SERVOMOVE;
    pwm.setPWM(1, 0, servo1PWM);
  }
  else if(gForceZ < -(ZBOUND)){
    servo2PWM = servo2PWM - SERVOMOVE;
    pwm.setPWM(2, 0, servo2PWM);
  }
  */
  delay(25);
}

void setupMPU(){
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processAccelData();
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}

void printData() {
  Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print(" Z=");
  Serial.println(gForceZ);
}
