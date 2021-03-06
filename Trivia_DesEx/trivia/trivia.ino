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


char network[] = "345E8B";
char password[] = "13080334";

char user[] = "Malobika";


char *trivia_q[500];
char *answer[100];


const int RESPONSE_TIMEOUT = 6000;
const int GETTING_PERIOD = 2000;
const int BUTTON_TIMEOUT = 1000;
const uint16_t IN_BUFFER_SIZE = 10000;
const uint16_t OUT_BUFFER_SIZE = 10000;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];
char *request_history[20];

char score_request_buffer[IN_BUFFER_SIZE];
char score_buffer[OUT_BUFFER_SIZE];


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
        
        if(count1 == 1){
          tft.setCursor(0,15,1);
          checker("True",*answer);
          }
        else if(count1 == 2){
          tft.setCursor(0,15,1);
          checker("FALSE",*answer);
          //do http post 
               
          }
        else if(count1 == 3){
          question();
          } 
                             
        count1 = 0;
        state1 = IDLE;
      }
      break;
  }
}

void post_scores(char* user, int correct, int wrong){

  char body[1000];
  sprintf(body, "user=%s&correct=%d&wrong=%d", user, correct, wrong);
  int body_len = strlen(body);
  sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/mfsyed/trivia/trivia.py HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev-2.net\r\n");
  strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
  sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
  strcat(request_buffer, "\r\n"); //new line from header to body
  strcat(request_buffer, body); //body
  strcat(request_buffer, "\r\n"); //header
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  /*
  strcat(score_request_buffer,"Host: 608dev-2.net\r\n");
  strcat(score_request_buffer, "Content-Length: application/x-www-form-urlencoded\r\n");
  sprintf(score_request_buffer + strlen(score_request_buffer), "Content-Length: %d\r\n", body_len);
  strcat(score_request_buffer,"\r\n"); //blank line
  strcat(score_request_buffer, body);
  strcat(score_request_buffer,"\r\n"); //blank line
  do_http_request("608dev-2.net", score_request_buffer, score_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);*/

  tft.setCursor(0,65,1);
  tft.println(response_buffer);
  
}



void question(){
  tft.fillScreen(TFT_WHITE);
  sprintf(request_buffer, "GET http://608dev-2.net/sandbox/sc/mfsyed/trivia/trivia.py HTTP/1.1\r\n");
  strcat(request_buffer,"Host: 608dev-2.net\r\n");
  strcat(request_buffer,"\r\n"); //blank line
  do_http_GET("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);

  *trivia_q = strtok(response_buffer, "`^`");
  *answer = strtok(NULL, "`^`");
  Serial.println(*trivia_q);
  Serial.println(*answer);
  tft.setCursor(0,5,1);
  tft.println(*trivia_q);
}


void checker(char* input, char* actual){
  if (strlen(input)+1==strlen(actual)){
    tft.setCursor(0,30,1);
    tft.println("CORRECT");
    post_scores(user, 1, 0);
  }

  else{
    tft.setCursor(0,30,1);
    tft.println("WRONG");
    post_scores(user, 0, 1);    
  }
}
