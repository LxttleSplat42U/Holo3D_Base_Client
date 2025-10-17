#include "HardwareSoftwareConfig.h"
/* File used to change based on hardware and software configuration*/
const int FAN_ID = 20; //Set the fan ID based on the fan number "X" with a trailing 0 [Format X0, Ex. if fan 3 ID=30]

// Pin definitions
const int RPM_SENSOR = 1; // Hall-effect or rpm sensor pin
const int motorPWM = 8; // Motor Control PWM pin
const int Monitor_LED = D13;  // Built in LED
bool Monitor_LED_State = false;

// Wifi Config [Must match the main fan network AP credentials]
const char* ssid = "Holo3D";     // WiFi network name
const char* password = "";      // WiFi password
/* More config details in WebSockets.h if IP adress differs from default 192.165.4.1*/

// WebSocket server details [must match main AP]
const char* websocket_server = "192.168.4.1";    // Replace with your WebSocket server IP
const int websocket_port = 80;                   // Replace with your WebSocket server port
const char* websocket_path = "/ws";              // Replace with your WebSocket path

// RPM Measurement
const int tRPMDebounceTime = 1000;    // [µs] Debounce time for RPM_sensor 
const int RPMTimerPrescaler = 80;     // Prescaler value to divide 80 Mhz PLL clock [ensure no decimal for precise timing] [<41.67ms based on 1440rpm (60/1440)]
const int RPM_MEASUREMENT_TIME = 1;  // Desired timer frequency at which to calculate the fan rpm [ensure no decimal for precise timing]
const int tReportInterval = 2000;   // [ms] Approximate interval at which RPM is reported to the fan blade
unsigned long int tReport = 0;    // Keeps track of last reporting time

//PWM Settings
const int freq = 8000; // Frequency [Hz]
const int resolution = 8; // bits


// Shutdown
bool shutDown = false;
int tshutdownTimer = 0;
int tshutDownRepeatInterval = 500; 

void LED_Blink(){
    Monitor_LED_State = !Monitor_LED_State;
    digitalWrite(Monitor_LED, Monitor_LED_State ? HIGH : LOW);
}

void setFanSpeed(uint8_t speed){
  ledcWrite(motorPWM, speed); //Duty Cycle (0-255)
}

void shutDownSystem(){
  setFanSpeed(0); // Shutdown fan motors
}