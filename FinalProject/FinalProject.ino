#include <Adafruit_Sensor.h> 
#include <Adafruit_ADXL345_U.h>
#include <Wire.h>
#include "MAX30102_PulseOximeter.h"
#include <BlynkSimpleEsp8266.h>

#define REPORTING_PERIOD_MS 1000
PulseOximeter pox;
uint32_t tsLastReport = 0;
int BPM;
int SpO2;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified();
int steps = 0;
float prev_magnitude = 0;
float threshold = 1.5;
bool prevAboveThreshold = false;

char auth[] = "vKenIR6pxw3Gps0TYIiajcad6K_QB-4S";         
char ssid[] = "HCMUTE";  
char pass[] = "88888888";     

void readMAX30102() {
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    BPM = pox.getHeartRate();
    SpO2 = (int) pox.getSpO2();
    Serial.print(BPM);
    Serial.print(", ");
    Serial.print(SpO2);
    Serial.println();
    Blynk.virtualWrite(V0, BPM);
    Blynk.virtualWrite(V1, SpO2);
    tsLastReport = millis();
  } 
}

void setup()
{
    Serial.begin(9600);
    Serial.print("Initializing..");
    Blynk.begin(auth, ssid, pass);
    delay(3000);
    if(!accel.begin())
    {
        Serial.println("No ADXL345 sensor detected.");
        while(1);
    }
   accel.setRange(ADXL345_RANGE_16_G);
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    }
    pox.setIRLedCurrent(MAX30102_LED_CURR_7_6MA);

}

float getAcc() {
  sensors_event_t event; 
  accel.getEvent(&event);
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;
  float magnitude = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
  return magnitude;
}

void loop()
{
  readMAX30102();
  float acc1 = getAcc();
  delay(180);
  float acc2 = getAcc();
  bool aboveThreshold = acc2 - acc1 > threshold;
  if (aboveThreshold && !prevAboveThreshold) {
    steps++;
    Blynk.virtualWrite(V3, steps);
  }
  prevAboveThreshold = aboveThreshold;
}

void readSensorSimulator() {
  int minBPM = 60, maxBPM = 140;
  int minSpO2 = 90, maxSpO2 = 100;
  int BPM = rand() % (maxBPM - minBPM + 1) + minBPM;
  int SpO2 = rand() % (maxSpO2 - minSpO2 + 1) + minSpO2;
  Blynk.virtualWrite(V0, BPM);
  Blynk.virtualWrite(V1, SpO2);
  Serial.print(BPM);
  Serial.print(", ");
  Serial.print(SpO2);
  Serial.println();
}