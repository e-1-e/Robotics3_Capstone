#include "FastIMU.h"
#include <Wire.h>
#include <WiFi.h>
#include <WifiAP.h>
#include <ArduinoJson.h>

const char *ssid = "ap lang except ts vegetables";
const char *password = "ooh la la la";

#define IMU_ADDRESS 0x68    //Change to the address of the IMU
//#define PERFORM_CALIBRATION //Comment to disable startup calibration
MPU6050 IMU;               //Change to the name of any supported IMU! 

// Currently supported IMUS: MPU9255 MPU9250 MPU6886 MPU6500 MPU6050 ICM20689 ICM20690 BMI055 BMX055 BMI160 LSM6DS3 LSM6DSL QMI8658

calData calib = { 0 };  //Calibration data
AccelData accelData;    //Sensor data
GyroData gyroData;
MagData magData;

WiFiServer server(80);


void wifiSetupStuff(){
  Serial.println("Configurin access point...");
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Soft AP creation failed.");
    while(1);

  }

   IPAddress myIP = WiFi.softAPIP();
   WiFi.setSleep(false);
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.begin();
}

void accelerometerStuff(){
  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
    while (true) {
      ;
    }
  }

  pinMode(46, INPUT_PULLUP);
  pinMode(10, INPUT);
  pinMode(12, INPUT);
  
#ifdef PERFORM_CALIBRATION
  Serial.println("FastIMU calibration & data example");
  if (IMU.hasMagnetometer()) {
    delay(1000);
    Serial.println("Move IMU in figure 8 pattern until done.");
    delay(3000);
    IMU.calibrateMag(&calib);
    Serial.println("Magnetic calibration done!");
  }
  else {
    delay(5000);
  }

  delay(5000);
  Serial.println("Keep IMU level.");
  delay(5000);
  IMU.calibrateAccelGyro(&calib);
  Serial.println("Calibration done!");
  Serial.println("Accel biases X/Y/Z: ");
  Serial.print(calib.accelBias[0]);
  Serial.print(", ");
  Serial.print(calib.accelBias[1]);
  Serial.print(", ");
  Serial.println(calib.accelBias[2]);
  Serial.println("Gyro biases X/Y/Z: ");
  Serial.print(calib.gyroBias[0]);
  Serial.print(", ");
  Serial.print(calib.gyroBias[1]);
  Serial.print(", ");
  Serial.println(calib.gyroBias[2]);
  if (IMU.hasMagnetometer()) {
    Serial.println("Mag biases X/Y/Z: ");
    Serial.print(calib.magBias[0]);
    Serial.print(", ");
    Serial.print(calib.magBias[1]);
    Serial.print(", ");
    Serial.println(calib.magBias[2]);
    Serial.println("Mag Scale X/Y/Z: ");
    Serial.print(calib.magScale[0]);
    Serial.print(", ");
    Serial.print(calib.magScale[1]);
    Serial.print(", ");
    Serial.println(calib.magScale[2]);
  }
  delay(5000);
  IMU.init(calib, IMU_ADDRESS);
#endif

  //err = IMU.setGyroRange(500);      //USE THESE TO SET THE RANGE, IF AN INVALID RANGE IS SET IT WILL RETURN -1
  //err = IMU.setAccelRange(2);       //THESE TWO SET THE GYRO RANGE TO ±500 DPS AND THE ACCELEROMETER RANGE TO ±2g
  
  if (err != 0) {
    Serial.print("Error Setting range: ");
    Serial.println(err);
    while (true) {
      ;
    }
  }
}

void setup() {
  Wire.begin(17, 18);
  Wire.setClock(400000); //400khz clock
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  accelerometerStuff();
  wifiSetupStuff();
}

void kodiPrintData(){
  Serial.println("Joy X: " + String(analogRead(12)) + ". Joy Y: " + String(analogRead(10)) + ". Joy?: " + String(digitalRead(46)));
  Serial.print(accelData.accelX);
  Serial.print("\t");
  Serial.print(accelData.accelY);
  Serial.print("\t");
  Serial.print(accelData.accelZ);
  Serial.print("\t");
  
  Serial.print(gyroData.gyroX);
  Serial.print("\t");
  Serial.print(gyroData.gyroY);
  Serial.print("\t");
  Serial.print(gyroData.gyroZ);

}

void loop() {
  IMU.update();
  IMU.getAccel(&accelData);
  IMU.getGyro(&gyroData);
  
  //kodiPrintData();
  
  delay(5);

  JsonDocument bob;
  JsonArray joyStickStuff = bob["joyStickStuff"].to<JsonArray>();
  joyStickStuff.add(analogRead(12));
  joyStickStuff.add(analogRead(10));
  joyStickStuff.add(digitalRead(46));
  JsonArray accelGyroStuff = bob["accelGyroStuff"].to<JsonArray>();
  accelGyroStuff.add(accelData.accelX);
  accelGyroStuff.add(accelData.accelY);
  accelGyroStuff.add(accelData.accelZ);
  accelGyroStuff.add(gyroData.gyroX);
  accelGyroStuff.add(gyroData.gyroY);
  accelGyroStuff.add(gyroData.gyroZ);

  String jsoresult = "";
  serializeJson(bob, jsoresult);

  

  WiFiClient client = server.available();
  if (client) {
    Serial.println("OH WOAH!");
    String x = client.readStringUntil('\r');
    Serial.println(x);
    // Write response headers
    client.println(F("HTTP/1.0 200 OK"));
    client.println(F("Content-Type: application/json"));
    client.println(F("Connection: close"));
    client.print(F("Content-Length: "));
    client.println(measureJsonPretty(bob));
    client.println();

    // Write JSON document
    serializeJsonPretty(bob, client);

    // Disconnect
    client.stop();
  } else {
    Serial.println(jsoresult);
  }


}
