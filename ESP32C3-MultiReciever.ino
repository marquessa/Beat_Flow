/*********
  Marquessa Mackenzie
  April 2024
  Complete project at https://marquessa.art/beat-flow
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  This code is written for an ESP32C3 mounted on a SEEED Studio Expansion 
  Board Base for XIAO. See the full project for more info.
*********/

// Required libraries
#include <esp_now.h>
#include <WiFi.h>
#include <BleKeyboard.h>
#include <U8x8lib.h>
#include <Wire.h>

#define PIN_WIRE_SCL D5  // pin connected to clock
#define PIN_WIRE_SDA D4  // pin connected to data

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* clock=*/PIN_WIRE_SCL, /* data=*/PIN_WIRE_SDA, /* reset=*/U8X8_PIN_NONE);  // OLED display constructor

// Create BLE keyboard object
BleKeyboard bleKeyboard("Beat_Flow");

// Left Sensor Values
int analogValueLeft;
int analogMinLeft = 100;   // minimum sensor value
int analogMaxLeft = 3800;  // maximum sensor value

// Right Sensor Values
int analogValueRight;
int analogMinRight = 100;   // minimum sensor value
int analogMaxRight = 3000;  // maximum sensor value

// Key Press Threshold Left
int pressThresholdLeft = 2700;  // value for triggering threshold

// Key Press Threshold Right
int pressThresholdRight = 2500;  // value for triggering threshold

// Key Press Current State Left
bool currentLeftState;  // value of current press
bool prevLeftState;     // value of previous press

// Key Press Current State Right
bool currentRightState;  // value of current press
bool prevRightState;     // value of previous press


// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  int leftValue;
  int rightValue;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message leftBoard;
struct_message rightBoard;

// Create an array with all the structures
struct_message boardsStruct[2] = { leftBoard, rightBoard };

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  boardsStruct[myData.id - 1].leftValue = myData.leftValue;
  boardsStruct[myData.id - 1].rightValue = myData.rightValue;
}

void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);

  //Initialize Keyboard
  bleKeyboard.begin();

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  u8x8.begin();                               // start display
  u8x8.setFont(u8x8_font_chroma48medium8_r);  // Set a suitable font. This font will be used for U8x8log
  u8x8.setFlipMode(1);                        // 0: Default position, 1: flip 180 deg

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  {
    u8x8.setCursor(0, 1);
    u8x8.print("Beat_Flow");
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  // calibration();
}

void loop() {

  // Acess the variables for each board
  int leftBoard = boardsStruct[0].leftValue;
  int rightBoard = boardsStruct[1].rightValue;

  /* sanity check
  Serial.print("Left Sensor Value:");
  Serial.print(leftBoard);
  Serial.print("\t");
  Serial.print("Right Sensor Value:");
  Serial.print(rightBoard);
  Serial.println(); 
  */

  // arguement for bool of sensor press state
  if (bleKeyboard.isConnected()) {
    if (leftBoard >= pressThresholdLeft) {
      currentLeftState = 1;
    } else {
      currentLeftState = 0;
    }

    if (rightBoard >= pressThresholdRight) {
      currentRightState = 1;
    } else {
      currentRightState = 0;
    }

    // left sensor to trigger L keypress
    if (currentLeftState == 1 && prevLeftState == 0) {
      Serial.print("Left Sensor Value:");
      Serial.print("\t");
      Serial.println(leftBoard);
      Serial.print("\n");

      bleKeyboard.write('l');  // send a keypress

      // print value on LCD display
      u8x8.clearLine(3);
      u8x8.setCursor(0, 3);
      u8x8.print("L Value: ");
      u8x8.setCursor(8, 3);
      u8x8.print(leftBoard);
    } else {
      u8x8.clearLine(3);
      u8x8.setCursor(0, 3);
      u8x8.print("L Value: ");
      u8x8.setCursor(8, 3);
      u8x8.print(leftBoard);
    }

    // right sensor to trigger R keypress
    if (currentRightState == 1 && prevRightState == 0) {
      Serial.print("Right Sensor Value:");
      Serial.print("\t");
      Serial.println(rightBoard);
      Serial.print("\n");

      bleKeyboard.write('r');  // send a keypress

      // print value on LCD display
      u8x8.clearLine(4);
      u8x8.setCursor(0, 4);
      u8x8.print("R Value: ");
      u8x8.setCursor(8, 4);
      u8x8.print(rightBoard);
    } else {
      u8x8.clearLine(4);
      u8x8.setCursor(0, 4);
      u8x8.print("R Value: ");
      u8x8.setCursor(8, 4);
      u8x8.print(rightBoard);
    }

    delay(50);
    prevLeftState = currentLeftState;    // redefine current state value to reset keypress
    prevRightState = currentRightState;  // redefine current state value to reset keypress
  }
}