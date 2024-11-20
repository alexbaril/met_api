/**************************************************************************
HTTP Request Example
Fetch the Met department names and show them on the display

To fetch the departments, press either button on the LILYGO (GPIO 0 or 35)     
**************************************************************************/
#include "TFT_eSPI.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// TODO: replace with your own SSID & Password
const char* ssid = "Columbia University";
const char* password = "";

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35

volatile bool leftButtonPressed = false;
volatile bool rightButtonPressed = false;

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);
  
  // setup our display
  tft.init();
  tft.setRotation(1);  // 1 = landscape, 2 = portrait
  tft.fillScreen(TFT_BLACK);

  Serial.print("display dimensions: ");
  Serial.print(tft.width());
  Serial.print(" x ");
  Serial.println(tft.height());

  // connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Send the HTTP POST request
  if (WiFi.status() == WL_CONNECTED) {
    fetchDepartments();
  } else {
    Serial.println("WiFi not connected");
  }

  // setup our buttons
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), pressedLeftButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), pressedRightButton, FALLING);
}

void fetchDepartments() {
    Serial.println("fetch departments");
    HTTPClient http;

    String url = "https://collectionapi.metmuseum.org/public/collection/v1/departments";
    http.begin(url);

    // Set the content type to JSON
    http.addHeader("Content-Type", "application/json");

    // Send the request
    int httpResponseCode = http.GET();

    // Check the response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);

      JSONVar responseJSON = JSON.parse(response);

      if (JSON.typeof(responseJSON) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

     Serial.println(responseJSON);

     JSONVar keys = responseJSON.keys();


     JSONVar value = responseJSON[keys[0]];
     Serial.print(keys[0]);
     Serial.print(" = ");
     Serial.println(value);

     for (int i = 0; i < 19; i++)
     {
      //Serial.println(value[i]);
      String department = JSON.stringify(value[i]);
      int comma = department.indexOf(',');
      String sub = department.substring(comma);
      int colon = sub.indexOf(':');
      String sub2 = sub.substring(colon);
      int first = sub2.indexOf('"');
      int last = sub2.lastIndexOf('"');
      String name = sub2.substring(first+1,last);
      Serial.println(name);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawString(name, 2, 20);
      delay(1000);
      tft.fillScreen(TFT_BLACK);
     }


    

      
     

    }
    // Free resources
    http.end();
}

void pressedLeftButton() {
  leftButtonPressed = true;
}

void pressedRightButton() {
  rightButtonPressed = true;
}

void loop() {
  // fetch departments when either button is pressed
  if (leftButtonPressed) {
    fetchDepartments();
    leftButtonPressed = false;
  }
  if (rightButtonPressed) {
    fetchDepartments();
    rightButtonPressed = false;
  }
}

