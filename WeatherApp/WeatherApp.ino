#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.



TFT_eSPI tft = TFT_eSPI();  //Make instance of TFT object




#define IDLE 0
#define PUSH 1
#define RELEASE 2
#define CYCLE 3


int count1=0;
uint8_t state1;


char network[] = "MIT";
char password[] = "";




const int RESPONSE_TIMEOUT = 6000;
const int GETTING_PERIOD = 2000;
const int BUTTON_TIMEOUT = 1000;
const uint16_t IN_BUFFER_SIZE = 10000;
const uint16_t OUT_BUFFER_SIZE = 10000;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];
char *request_history[20];



const uint8_t INPUT_PIN1 = 16; //pin connected to button
uint8_t button1 = digitalRead(INPUT_PIN1);

unsigned long timer1;
unsigned long timer2;  
uint32_t timer; //used for loop timing
const uint16_t LOOP_PERIOD = 50; //period of system

const int BUFFER_LENGTH = 200;  //size of char array we'll use for
char buffer[BUFFER_LENGTH] = {0}; //dump chars into the 


void setup() {
  Serial.begin(115200);
  tft.init(); 
  tft.setRotation(2); 
  tft.setTextSize(1);
  tft.fillScreen(TFT_WHITE); 
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  Serial.begin(115200); 
  delay(100);
  pinMode(INPUT_PIN1, INPUT_PULLUP);

  menu();

  state1 = IDLE;

  for(int i = 0; i < 20; i++){
    request_history[i] = "";
  }



  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
    }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str() , WiFi.SSID().c_str());
    delay(500);
    }
  else { 
    //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
    }

  
}

void loop() {
  tft.setCursor(0,0,1);
  button1 = digitalRead(INPUT_PIN1);
  counter();
  
}


void menu(){
  tft.setCursor(0,0,1);
  tft.println("1: Temperature.");
  tft.setCursor(0,25,1);
  tft.println("2: Time.");
  tft.setCursor(0,50,1);
  tft.println("3: Date.");
  tft.setCursor(0,75,1);
  tft.println("4: Weather.");
  tft.setCursor(0,100,1);
  tft.println("5: Humidity.");
  tft.setCursor(0,125,1);
  tft.println("6: Pressure.");
}







void counter(){
  switch(state1){
    case IDLE:
      if (button1 == 0){
        state1 = PUSH;
        timer2 = millis();
      }
      break; 
      
    case PUSH:
      if (button1 == 1 && millis()-timer2 > 100){
        state1 = RELEASE;
        timer2 = millis();
      }
      break;
      
    case RELEASE:
      timer1 = millis();
      count1 = count1 + 1;
      state1 = CYCLE;
      break;
      
    case CYCLE:
      if (millis() - timer1 < 1000){
        if (button1 == 0 && millis()-timer2 > 100){
          state1 = PUSH;
        }
      }
      else{
        tft.fillScreen(TFT_WHITE);
        tft.setCursor(0,5,1);
        tft.println(count1);
        
        if(count1 == 1){
          information("TEMP");
          }
        else if(count1 == 2){
          information("TIME");
          }
        else if(count1 == 3){
          information("DATE");
          } 
        else if(count1 == 4){
          information("WEATHER");
          } 
        else if(count1 == 5){
          information("HUMIDITY");
          } 
        else if(count1 == 6){
          information("PRESSURE");
          } 
                             
        count1 = 0;
        state1 = IDLE;
      }
      break;
  }
}







void information(char *response_line){
  int x = 0;
  Serial.println(response_line);
  tft.fillScreen(TFT_WHITE);
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/mfsyed/weatherapp/weatherapp.py?input=%s%s", response_line, " HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"\r\n"); //blank line
  do_http_GET("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
  Serial.println(response_buffer);

  for (int i = 19; i > 0; i--){
    request_history[i] = request_history[i-1];
  }

  request_history[0] = new char[100];
  strcpy(request_history[0], response_buffer);

  for (int i =0; i < 20; i++){
    tft.setCursor(0,x,1);
    tft.println(request_history[i]);
    x = x + 20;
  } 
}
















uint8_t char_append(char* buff, char c, uint16_t buff_size) {
        int len = strlen(buff);
        if (len>buff_size) return false;
        buff[len] = c;
        buff[len+1] = '\0';
        return true;
}

void do_http_GET(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response,client.read(),OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}   
