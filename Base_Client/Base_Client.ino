#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h> // Install "WebSockets" by Markus Sattler from Library Manager
#include "driver/pulse_cnt.h" 
#include "esp_err.h"

const int FAN_ID = 20; //Set fan ID
IPAddress local_IP(192, 168, 4, FAN_ID); // Static IP address for this ESP32
IPAddress gateway(192, 168, 4, 1); // Gateway IP address
IPAddress subnet(255, 255, 255, 0); // Subnet mask

//Pin definitions
uint8_t SpeedSensorPin = 1;

//Speed RPM [Position Sensor]
#define SPEED_SENSOR_PIN 1      // Speed Sensor pin number
#define SAMPLE_INTERVAL_US 1000  // 1 ms = 1000 Hz sampling
const int LOW_TH  = 3100;  // ADC level if no blade passed
const int HIGH_TH = 3120;  // ADC level if blade passed

volatile bool bladePassed = false;
volatile int nBladePasses = 0; //Count number of times the blade has passed

hw_timer_t * TIM0; //Setup hardware timer for accurate and precise timing
int currFanSpeed = 0; //rpm of the fan currently


//Motor Control
const int motorPWM = 8;

//PWM Settings
const int freq = 8000; //Frequency [Hz]
const int resolution = 8; //bits

//User command variables
uint8_t msgSpeed = 0; //Fan Speed 0 - 100


const int Monitor_LED = D13;
bool Monitor_LED_State;
int tblink = 0; 
int tReport = 0;
bool Client_Connected = false;

//WiFi credentials for connecting to existing network
const char* ssid = "Holo3D";     // WiFi network name
//const char* password = "YOUR_WIFI_PASSWORD"; // WiFi password

// WebSocket server details
const char* websocket_server = "192.168.4.1";    // Replace with your WebSocket server IP
const int websocket_port = 80;                   // Replace with your WebSocket server port
const char* websocket_path = "/ws";              // Replace with your WebSocket path

//Setup WebSocket Client
WebSocketsClient webSocket;

// put function declarations here:
void SetupInterrupts();

void initWiFiStation(); //Initialize WiFi in station mode
void initWebSocketClient(); //Initialize WebSocket client
void LED_Blink(); //Used to check if board is reponsive/active by monitoriung the built in D13 LED flashing
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length); //WebSocket Event Handler
void setFanSpeed(uint8_t speed);

void handleBladeInterrupt();
void handleTIM0();

void SetupInterrupts(){


  //Timer
  TIM0 = timerBegin(25600); //3125 Hz timer (Timer_number, prescaler, upcount?)
  timerAttachInterrupt(TIM0, handleTIM0); //Timer, function to call
  timerAlarm(TIM0, 3125, true, 0); //Timer, ARR, ARR_Enabled?, number of times? (0 = infinite)
}


void setup() {
  //PWM & Motor Control Setup
  pinMode(motorPWM, OUTPUT);  //Ensure Motor is OFF
  digitalWrite(motorPWM, LOW);
  ledcAttach(motorPWM, freq, resolution);

  //Enable Timer and Interrupts
  SetupInterrupts();  

  // put your setup code here, to run once:
  Serial.begin(115200);
  initWiFiStation();
  initWebSocketClient();

  //Set initial Built-in LED state
  pinMode(Monitor_LED, OUTPUT);
  digitalWrite(Monitor_LED, LOW);
  Monitor_LED_State = false;
}

void loop() {
  // Handle WebSocket client events
  webSocket.loop();
  
  // Send periodic updates if connected
  if (Client_Connected){
    if (millis() - tReport > 2000) {
      tReport = millis();
      
      // Create JSON-like message with fan data (without external JSON library)
      String message = "{";
      message += "\"fan_id\":" + String(FAN_ID) + ",";
      message += "\"rpm\":" + String(currFanSpeed) + ",";
      message += "\"speed_setting\":" + String(msgSpeed) + ",";
      message += "\"sensor_reading\":" + String(analogRead(SpeedSensorPin));
      message += "}";
      
      //webSocket.sendTXT(message);
    }
  }
}

// put function definitions here:
void initWiFiStation(){
  // Configure static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  //Connect to existing WiFi network
  WiFi.begin(ssid); //add password if required
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    LED_Blink();
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
}

void initWebSocketClient(){
  webSocket.begin(websocket_server, websocket_port, websocket_path);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  Serial.println("WebSocket client initialized");
}

void LED_Blink(){
    Monitor_LED_State = !Monitor_LED_State;
    digitalWrite(Monitor_LED, Monitor_LED_State ? HIGH : LOW);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("WebSocket Disconnected");      
      setFanSpeed(0); //Switch off fan immediately if disconnected!
      Client_Connected = false;
      break;
      
    case WStype_CONNECTED:
      Serial.printf("WebSocket Connected to: %s\n", payload);
      Client_Connected = true;
      // Send initial to register as client with a unique ID      
      webSocket.sendTXT("REGISTER:" + String(FAN_ID));   
      Serial.println("Sent registration message: " + String(FAN_ID));      
      break;
      
    case WStype_TEXT:
      {
        Serial.printf("Received: %s\n", payload);
        
        // Process incoming data
        String msg = String((char*)payload);
        if (msg == "-1"){ //Shutdown command
          setFanSpeed(0);
        }
        if (msg.startsWith("MOTOR_SPEED:") ){
          String msgValue = msg.substring(strlen("MOTOR_SPEED:"));
          msgSpeed = msgValue.toInt(); //0 - 255
          setFanSpeed(msgSpeed);
        }
      }
      break;
      
    default:
      break;
  }
}

void setFanSpeed(uint8_t speed){
  ledcWrite(motorPWM, speed); //Duty Cycle (0-255)
}

//Function to calculate the current fan speed at consistent/known intervals
void handleTIM0(){

  int sensorValue = analogRead(SPEED_SENSOR_PIN);

  if (!bladePassed && sensorValue >= HIGH_TH){
    bladePassed = true;
    nBladePasses++;
  } else if (bladePassed && sensorValue <= LOW_TH){
    bladePassed = false;
  }

  currFanSpeed = nBladePasses;        //get the fan speed (divide by time 1s)
//nBladePasses = 0; //Reset counter
}