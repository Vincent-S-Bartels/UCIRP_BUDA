#include  <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LSM303.h>
#include <SPI.h>
#include <SD.h>

//Clocks
unsigned long tick = 0;
unsigned long previousMicro = 0;
unsigned long currentMicro = 0;
unsigned long sampPeriodMicro = 100;
unsigned long microsPassed = 0;
// Num Mins * 60 secs/min * 1000 ms/sec
unsigned long stopTime = 30000;

LSM303 accel;
float accelScale = 1615.23;
int accXInt;
int accYInt;
int accZInt;

//SD Card
File myFile;
bool inLoop = true;
char text[80];

//MLX90614
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
int objTempInt = 0;

const int MPU = 0x68;
float AccX, AccY, AccZ;

int mpuXInt = 0;
int mpuYInt = 0;
int mpuZInt = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  //Serial.println("Hello World");

  Wire.begin();
  Wire.setClock(50000);
  //Serial.println("Wire Setup");

  while (!mlx.begin());
  //Serial.println("MLX Setup");

  accel.init();
  accel.enableDefault();
  //Serial.println("LSM Setup");

  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  delay(20);

  SD.begin(10);

  currentMicro = micros();
}

void loop() {
  Serial.println("Entered Loop");
  Serial.println(currentMicro);
  myFile = SD.open("DATA.CSV", FILE_WRITE);
  myFile.println("Micro, LSM X, LSM Y, LSM Z, MPU X, MPU Y, MPU Z, ObjTemp");
  while (stopTime > millis())  {
    currentMicro = micros();
    microsPassed = currentMicro - previousMicro;
    if (microsPassed >= sampPeriodMicro) {
      previousMicro = currentMicro;
      Wire.beginTransmission(MPU);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU, 6, true);
      AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
      AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
      AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;   
      accel.read();
      accXInt = (int) (1000.0 * accel.a.x);
      accYInt = (int) (1000.0 * accel.a.y);
      accZInt = (int) (1000.0 * accel.a.z);
      mpuXInt = (int) (1000.0 *  AccX);
      mpuYInt = (int) (1000.0 *  AccY);
      mpuZInt = (int) (1000.0 * AccZ);
      myFile.print(currentMicro);
      sprintf(text, ", %d, %d, %d, %d, %d, %d, %d",
              accXInt, accYInt, accZInt,
              mpuXInt, mpuYInt, mpuZInt,
              objTempInt);
      myFile.println(text);
      tick = tick  + 1;
    }
    if (tick % 1000 == 0) {
      objTempInt = (int) (mlx.readObjectTempC());
    }
  }
  myFile.close();
  Serial.println("Finished Data Collection");
  Serial.println(tick);
  while (1);
}
