#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.



//variables I used
char user[] = "Malobika";
char *red_str[5];
char *green_str[5];
char *blue_str[5];
char *user_str[100];
int r;
int b;
int g;
uint32_t counter; //used for timing
const uint32_t red = 0; //hardware pwm channel used in secon part
const uint32_t green = 13;
const uint32_t blue = 15;

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 2000; //periodicity of getting a number fact.
const int BUTTON_TIMEOUT = 1000; //button timeout in milliseconds
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

char network[] = "345E8B";  //SSID for 6.08 Lab
char password[] = "13080334"; //Password for 6.08 Lab


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //begin serial comms
  delay(100);
  
  WiFi.begin(network,password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  
  while (WiFi.status() != WL_CONNECTED && count<6) {
    delay(500);
    Serial.print(".");
    count++;
  }
  
  delay(2000);
  
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    ESP.restart(); // restart the ESP (proper way)
  }

  
  ledcSetup(red, 50, 8);
  ledcSetup(green, 80, 8);
  ledcSetup(blue, 80, 8);
  
  ledcAttachPin(13, red); 
  ledcAttachPin(12, green);
  ledcAttachPin(14, blue);

  counter = millis();
}

void loop() {
  // put your main code here, to run repeatedly:

 lamp_get();

 if (millis() - counter > 100) {
  ledcWrite(red, r);
  ledcWrite(green, g);
  ledcWrite(blue, b);
 }

}
void lamp_get(){
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/mfsyed/Lamp/lamp.py?user=%s%s", user, " HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"\r\n"); //blank line
  do_http_GET("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);

  *user_str = strtok(response_buffer, "`^`");
  *red_str = strtok(NULL, "`^`");
  *green_str = strtok(NULL, "`^`");
  *blue_str = strtok(NULL, "`^`");
  
  r = atoi(*red_str); //converting string vals to ints
  g = atoi(*green_str);
  b = atoi(*blue_str);

  
}
