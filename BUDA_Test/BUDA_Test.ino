#include  <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LSM303.h>
#include <Adafruit_MPU6050.h>
#include <SPI.h>
#include <SD.h>
//Clocks
unsigned int tick = 0;
unsigned long previousMicro = 0;
unsigned long currentMicro = 0;
unsigned long sampPeriodMicro = 100;
unsigned long microsPassed = 0;
unsigned long stopTime = 0;
LSM303 accel;
float accX;
float accY;
float accZ;
float accelScale = 1615.23;
int accXInt;
int accYInt;
int accZInt;

//SD Card
File myFile;
float window = 20;
bool inLoop = true;
char text[80];

//MLX90614
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
float objTemp = 0;
int objTempInt = 0;
//MPU6050
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;
float mpuScale = 0;
int mpuXInt;
int mpuYInt;
int mpuZInt;

void setupMPU() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    sensors_event_t a, g, temp;
  }
  Serial.println("Setup MPU");
  delay(100);

}
void setupSerial() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Hello World!\nSerial setup.");
  delay(100);
}

void setupI2C() {
  Wire.begin();
  //Wire.setClock(50000); //set to 50khz because the temperature sensors max is 100k.
  Serial.println("Wire Setup");
  delay(100);
}

void setupMLX() {
  //Serial.println("Adafruit MLX90614 Test");
  if (!mlx.begin()) {
    Serial.println("Error Connecting MLX. Check wiring.");
  }
  Serial.println("Setup MLX");
  delay(100);
}
void setupAccel() {
  accel.init();
  accel.enableDefault();
  Serial.println("Setup Accel");
  delay(100);
}
void setupSD() {
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

}
void scaleMPU() {
  mpu.getEvent(&a, &g, &temp);
  mpuScale = a.acceleration.z / 9.81;
}
void setup() {
  setupSerial();
  setupI2C();
  setupMLX();
  //setupAccel();
  setupMPU();
  setupSD();
  Serial.flush();
  currentMicro = micros();
}

/*void getTempReading() {
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  }*/
void getAccel() {
  accel.read();
  accX = accel.a.x / accelScale;
  accY = accel.a.y / accelScale;
  accZ = (accel.a.z / accelScale);
}
void floatToInt() {
  accX = accX * 1000.0;
  accXInt = (int) accX;
  accY = accY * 1000.0;
  accYInt = (int) accY;
  accZ = accZ * 1000.0;
  accZInt = (int) accZ;

  mpuXInt = (int) (a.acceleration.x * 1000.0);
  mpuYInt = (int) (a.acceleration.y * 1000.0);
  mpuZInt = (int) (a.acceleration.z * 1000.0);

  objTemp = objTemp * 1000.0;
  objTempInt = (int) objTemp;
}

void loop() {
  myFile = SD.open("test_1.txt", FILE_WRITE);
  myFile.println("LSM X, LSM Y, LSM Z, MPU X, MPU Y, MPU Z, ObjTemp");
  Serial.println("Created Output File");
  while (inLoop) {
    currentMicro = micros();
    microsPassed = currentMicro - previousMicro;
    if (microsPassed >= sampPeriodMicro)
    {
      previousMicro = currentMicro;
      getAccel();
      mpu.getEvent(&a, &g, &temp);
      objTemp = mlx.readObjectTempC();
      Serial.println(text);
      Serial.print("LSM X: ");
      Serial.println(accX);
      Serial.print("MPU X: ");
      Serial.println(a.acceleration.x);
      floatToInt();
      /*sprintf(text, "%d, %d, %d, %d, %d, %d, %d ",
        accXInt, accYInt, accZInt,
        mpuXInt, mpuYInt, mpuZInt,
        objTempInt);
        Serial.println(text);
        myFile.println(text);*/
    }
    if (tick > 100) {
      inLoop = false;
    }
    else {
      tick = tick + 1;
    }
  }
  myFile.close();
  exit(0);
}
