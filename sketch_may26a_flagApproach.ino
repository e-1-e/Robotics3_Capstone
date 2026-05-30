#include <Servo.h>

#include <ArduinoJson.h>




Servo baseRotator;
Servo armHinger;
Servo ballGrabber;

int currentMotor = 1;
int baseRotPos = 0;
int armHinPos = 0;
int baseRotTarget = 0;
int armHinTarget = 0;
int baseRotVel = 0;
int armHinVel = 0;

int ballGrabPos = 0;
int ballGrabTarget = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  baseRotator.attach(3);
  armHinger.attach(5);
  ballGrabber.attach(6);

  baseRotator.write(baseRotPos);
  armHinger.write(180);
  ballGrabber.write(0);

  Serial.begin(115200);
  Serial.setTimeout(500);
  delay(3000);
  Serial.println("OMG SKIBIDI");
}


void testLoop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() && false) { //user commanded
    String command = (Serial.readStringUntil('\n'));
    if (command == "apple") {
      currentMotor = 1;
    } else if (command == "banana") {
      currentMotor = 2;
    } else {
      int bob = command.toInt();
      if (currentMotor == 1) {
        baseRotator.write(bob);
      } else {
        armHinger.write(bob);
      }
    }
  } else if (Serial.available()) { //test Arduino serial grabby
    String a = "";
    int incomingByte = 0;
    while ((char)incomingByte != '\n') {
      delay(1);
      if (Serial.available()) {
        incomingByte = Serial.read();
        a += (char)(incomingByte);
      }
      
    }
    
    Serial.print("i received ");
    Serial.println(a);
  }
  delay(50);
}

int clamp(int num, int min, int max) {
  if (num > min) {
    if (num > max) {
      return max;
    }
    return num;
  } else {
    return min;
  }
}

String grabbyInput(){
  String a = "";
  int incomingByte = 0;

  while ((char)incomingByte != '}') {
    
    if (Serial.available()) {
      incomingByte = Serial.read();
      //Serial.print((char)incomingByte);
      a += (char)(incomingByte);
    }
    
  }
  

  Serial.println("made it out? " + a);

  return a;
}


void loop() {
  //Serial.println("WOAH");

  //baseRotPos = (baseRotTarget) * 0.01 + (baseRotPos) * 0.99;
  armHinPos = (armHinTarget) * 0.65 + (armHinPos) * 0.35;
  ballGrabPos = (ballGrabTarget * 0.65) + (ballGrabPos * 0.35);
  //baseRotator.write(baseRotPos);
  armHinger.write(armHinPos);
  ballGrabber.write(ballGrabPos);

  // if (armHinPos > 165) {
  //   ballGrabber.write(180);
  // } else {
  //   for (int i = 0; i < 4; i++) {
  //     ballGrabber.write(0);
  //   }
  // }

  if (Serial.available()) {
    //Serial.println("ALIVE CHECK");
    String huh = grabbyInput();
    Serial.println("i got: " + huh);
    
    JsonDocument recvData;

    DeserializationError bobError = deserializeJson(recvData, huh);
    if (bobError){
      Serial.print("ERRORRROROROROROR: ");
      Serial.print(bobError.c_str());
      return;
    }

    huh = huh.substring(1, huh.length() - 1);

    int cmd1 = recvData["moveX"]; //between -500 and 500
    int cmd2 = recvData["moveY"]; //between -500 and 500
    int cmd3 = recvData["mode"];
    int cmd4 = recvData["buttonPress"];

    Serial.print("answers: ");
    Serial.print(cmd1);
    Serial.print(" ");
    Serial.print(cmd2);
    Serial.print(" ");
    Serial.print(cmd3);
    Serial.print(" ");
    Serial.print(cmd4);
    Serial.println(" ");

    if (cmd3 == 0) {
      if (cmd1 < 5) {
        cmd1 = 0;
      }

      if (cmd2 < 5) {
        cmd2 = 0;
      }

      baseRotPos = clamp(baseRotPos + cmd1 * 0.2, 0, 180);
      armHinPos = clamp(armHinPos + cmd2 * 0.2, 0, 180);
    } else {
      int scaleFactorX = 2;
      int scaleFactorY = 1;
      int moveX = map(cmd1, 0, 3800, 0, 10) - 5;
      int moveY = map(cmd2, 0, 3800, 0, 10) - 5;

      baseRotPos = clamp(baseRotPos + moveX/scaleFactorX, 0, 180);
      // armHinPos = clamp(armHinPos + moveY/scaleFactorY, 0, 180);
      //baseRotVel = moveX/scaleFactorX;
      armHinVel = moveY/scaleFactorY;
      //baseRotTarget = clamp(baseRotVel*10000,0,180);
      armHinTarget = clamp(armHinVel*10000,0,180);
      ballGrabTarget = clamp(moveX*10000, 0, 180);


      

      Serial.print("answers2: ");
      Serial.print(moveX);
      Serial.print(" ");
      Serial.print(moveY);
      Serial.print(" ");
      Serial.print(baseRotPos);
      Serial.print(" ");
      Serial.print(armHinPos);
      Serial.println(" ");
      Serial.print(ballGrabTarget);
      Serial.println(" ");
      Serial.print(ballGrabPos);
      Serial.println(" ");
    }

    //baseRotator.write(baseRotPos);
     //armHinger.write(armHinPos);
      
    Serial.println("READY");
    
  }
  
}
