/*********
  Marquessa Mackenzie
  April 2024
  Complete project at https://marquessa.art/beat-flow
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  This code is written for an NODEMCU-32S with an ESP32 chip. 
  See the full project for more info.
*********/

// Required libraries
#include <esp_now.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>


// Reciever esp MAC address
uint8_t broadcastAddress[] = { 0xD4, 0xF9, 0x8D, 0x04, 0x21, 0x64 };  // XIAO ESP32-C3 - D4:F9:8D:04:21:64

#define PIN_NEO_PIXEL 27  // NodeMCU-32S GPIO LED Pin
#define NUM_PIXELS 8      // The number of LEDs (pixels)

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);  // pixel constructor

int rightPin = 39;  // NodeMCU-32S GPIO Sensor Pin

int analogMin = 2300;      // minimum sensor value **customize as needed
int analogMax = 3000;      // maximum sensor value **customize as needed
int pressThreshold = 100;  // value for triggering threshold **customize as needed

bool currentRightState;  // value of current press
bool prevRightState;     // value of previous press

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id = 2;  // must be unique for each sender board
  int leftValue;
  int rightValue;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create peer interface
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery Fail");
    delay(5000);
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // start neopixels
  pixels.begin();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Set Pin
  pinMode(rightPin, INPUT_PULLUP);
}

void loop() {
  // Set values to send
  myData.id;
  myData.rightValue = analogRead(rightPin);  // read value from sensor

  pixelsRight();

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result != ESP_OK) {
    Serial.println("Error sending the data");
  }
  delay(50);
}

// function for pixels to light up according to how hard the sensor is pressed
void pixelsRight() {
  int rightLED = map(myData.rightValue, analogMin, analogMax, 0, NUM_PIXELS);  //use myData var to get analog value

  if (rightLED > 0)  // if led is less than '0' aka: LOW
  {
    for (int i = 0; i < rightLED; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 25, 25));  // light blue
    }
  } else {
    pixels.clear();  // set all pixel colors to 'off'
  }

  pixels.show();  // argument only takes effect if pixels.show() is called [REQUIRED]
}