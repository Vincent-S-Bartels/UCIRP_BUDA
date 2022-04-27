#include  <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LSM303.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
sensors_event_t a, g, temp;

unsigned int tick = 0;
LSM303 accel;
float accX;
float accY;
float accZ;


Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_MPU6050 mpu;
void setupMPU() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    sensors_event_t a, g, temp;
  }
}
void setupSerial() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Hello World!\nSerial setup.");
}
void setupOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Failded"));
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hi");
  display.display();
  delay(2000);
}
void setupI2C() {
  Wire.begin();
  Wire.setClock(50000); //set to 50khz because the temperature sensors max is 100k.
  Serial.println("Wire Setup");
}

void setupMLX() {
  Serial.println("Adafruit MLX90614 Test");
  if (!mlx.begin()) {
    Serial.println("Error Connecting MLX. Check wiring.");
  }
}
void setupAccel() {
  accel.init();
  accel.enableDefault();
}

void setup() {
  setupSerial();
  setupI2C();
  setupMLX();
  setupAccel();
  setupOLED();
  setupMPU();
  Serial.flush();
}

void getTempReading() {
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC());
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
}
void getAccel() {
  accel.read();
  accX = accel.a.x / 1640.775;
  accY = accel.a.y / 1640.775;
  accZ = (accel.a.z / 1640.775);
}
void displayReadings() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Obj T (C): ");
  display.println(mlx.readObjectTempC());

  display.print("LXM X:"); display.print(accX);
  display.print(" X2:"); display.println(a.acceleration.x);
  display.print("LSM Y:"); display.print(accY);
  display.print(" Y2:"); display.println(a.acceleration.y);
  display.print("LSM Z:"); display.print(accZ);
  display.print(" Z2:"); display.println(a.acceleration.z);
  display.display();
}
void loop() {
  if (tick % 10000 == 0) {
    getAccel();
    mpu.getEvent(&a, &g, &temp);
    displayReadings();
  }

  /* currentMillis = millis();
    millisPassed = currentMillis - previousMillis;
    if (millisPassed >= sampPeriod)
    {
     previousMillis = currentMillis;
     getAccel();
     Serial.print("Accel Z = ");
     Serial.println(accZ);

    }
  */


  tick = tick + 1;
  if (tick > 30000) {
    tick = 0;
  }
}
