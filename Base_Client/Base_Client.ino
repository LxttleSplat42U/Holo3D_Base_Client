#include "HardwareSoftwareConfig.h"
#include "WebSockets.h"
#include "RPM_Measurements.h"

// #include "driver/pulse_cnt.h" 
// #include "esp_err.h"

//Speed RPM [Position Sensor]
#define SPEED_SENSOR_PIN 1      // Speed Sensor pin number
#define SAMPLE_INTERVAL_US 1000  // 1 ms = 1000 Hz sampling
const int LOW_TH  = 3100;  // ADC level if no blade passed
const int HIGH_TH = 3120;  // ADC level if blade passed

volatile bool bladePassed = false;
volatile int nBladePasses = 0; //Count number of times the blade has passed

hw_timer_t * TIM0; //Setup hardware timer for accurate and precise timing
int currFanSpeed = 0; //rpm of the fan currently

//User command variables
uint8_t msgSpeed = 0; //Fan Speed 0 - 100
bool Client_Connected = false;

void setup() {
  //PWM & Motor Control Setup
  pinMode(motorPWM, OUTPUT);  //Ensure Motor is OFF
  digitalWrite(motorPWM, LOW);
  ledcAttach(motorPWM, freq, resolution);

  Serial.begin(115200); // Enable serial communication for debugging
  initWiFiStation();  // Connect to wifi AP of main fan
  initWebSocketClient();  // Connect and register device to websocket using FAN_ID

  //Set initial Built-in LED state
  pinMode(Monitor_LED, OUTPUT);
  digitalWrite(Monitor_LED, LOW);
  Monitor_LED_State = false;

  // Start RPM_Measurements
  setupRPMMeasurement();
  setupRPMTimer();
}

void loop() {
  // Handle WebSocket client events
  webSocket.loop();
  
  // Send periodic updates if connected
  // if (Client_Connected){
  //   if (millis() - tReport > 2000) {
  //     tReport = millis();
      
  //     // Create JSON-like message with fan data (without external JSON library)
  //     String message = "{";
  //     message += "\"fan_id\":" + String(FAN_ID) + ",";
  //     message += "\"rpm\":" + String(currFanSpeed) + ",";
  //     message += "\"speed_setting\":" + String(msgSpeed) + ",";
  //     message += "\"sensor_reading\":" + String(analogRead(SpeedSensorPin));
  //     message += "}";
      
  //     //webSocket.sendTXT(message);
  //   }
  // }

  if (millis() - tReport > tReportInterval) {
    tReport = millis();
    if (avgRPMCount >= 1){
      avgRPM = sumRPM / avgRPMCount;
      avgRPMCount = 0;
      sumRPM = 0;
    } else {
      avgRPM = 0;
    }
    
    // Send to fan blade with ID X1 where X is the FAN_ID without the trailing 0
    webSocket.sendTXT(String(FAN_ID + 1) + ":RPM:" + String(avgRPM) + ":"); // Send current RPM to fan
  }
}


