#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

// Replace with your network credentials

const String ESPWIFIHOSTNAME = "DasCoolsteAuto";
const char* ssid = "test";
const char* password = "test1234";

const byte TRYWIFICONNECT = 60;

// Create an instance of the server
ESP8266WebServer server(80);

// Define the number of NeoPixels and the pin they are connected to (D8)
const int numPixels = 7;  // Change this to match your setup
const int neoPixelPin = D8; // Change this to the appropriate GPIO pin
const int brightness = 50;

// Create an instance of the Adafruit NeoPixel library
Adafruit_NeoPixel pixels(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

bool colorLoopActive = true; // Flag to control the color loop

void setup() {
  // Initialize NeoPixels
  pixels.begin();
  pixels.setBrightness(brightness);
  setNeoPixelsColor(0, 0, 128);

  // Connect to Wi-Fi
  WiFi.hostname(ESPWIFIHOSTNAME.c_str());
  WiFi.begin(ssid, password);
  byte tryWifiCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    tryWifiCounter++;

    if(tryWifiCounter == TRYWIFICONNECT)
    {
      break;
    }
  }
  WiFi.status() == WL_CONNECTED ? Serial.println("Connected to WiFi") : Serial.println("WIFI NOT CONNECTED");

  // Define HTTP handlers
  server.on("/", HTTP_GET, handleRoot);
  server.on("/red", HTTP_GET, handleRed);
  server.on("/green", HTTP_GET, handleGreen);
  server.on("/blue", HTTP_GET, handleBlue);
  server.on("/yellow", HTTP_GET, handleYellow);
  server.on("/fhmint", HTTP_GET, handlefhmint);
  server.on("/setcolor", HTTP_POST, handleSetColor); // not working
  server.on("/colorloop", HTTP_GET, handleColorLoop);

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();

  if (colorLoopActive) {
    colorLoop();
  }
}

void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP8266!");
}

void handleRed() {
  colorLoopActive = false;
  setNeoPixelsColor(255, 0, 0); // Set NeoPixels to red
  server.send(200, "text/plain", "Color set to Red");
}

void handleGreen() {
  colorLoopActive = false;
  setNeoPixelsColor(0, 255, 0); // Set NeoPixels to green
  server.send(200, "text/plain", "Color set to Green");
}

void handleBlue() {
  colorLoopActive = false;
  setNeoPixelsColor(0, 0, 255); // Set NeoPixels to blue
  server.send(200, "text/plain", "Color set to Blue");
}

void handlefhmint() {
  colorLoopActive = false;
  setNeoPixelsColor(0, 177, 172); // Set NeoPixels to blue
  server.send(200, "text/plain", "Color set to FH Mint");
}

void handleYellow() {
  colorLoopActive = false;
  setNeoPixelsColor(255, 255, 0); // Set NeoPixels to blue
  server.send(200, "text/plain", "Color set to Yellow");
}

void handleSetColor() {
  if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
    int red = server.arg("r").toInt();
    int green = server.arg("g").toInt();
    int blue = server.arg("b").toInt();
    setNeoPixelsColor(red, green, blue);
    server.send(200, "text/plain", "Color set to RGB(" + String(red) + ", " + String(green) + ", " + String(blue) + ")");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void handleColorLoop() {
  colorLoopActive = !colorLoopActive; // Start the color loop
  server.send(200, "text/plain", "Color loop toggled");
}

void setNeoPixelsColor(int red, int green, int blue) {
  for (int i = 0; i < numPixels; i++) {
    pixels.setPixelColor(i, pixels.Color(red, green, blue));
  }
  pixels.show();
}

void colorLoop() {
  static uint16_t colorWheelIndex = 0;
  static const uint8_t brightness = 128;

  for (int i = 0; i < numPixels; i++) {
    int pixelHue = (i * 65536L / numPixels + colorWheelIndex) % 65536L;
    pixels.setPixelColor(i, pixels.ColorHSV(pixelHue, 255, brightness));
  }
  pixels.show();

  colorWheelIndex += 256;
  delay(10); // Delay for smoother animation
}