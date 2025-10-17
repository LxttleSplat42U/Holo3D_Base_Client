#pragma once
/* File used to change based on hardware and software configuration*/
extern const int FAN_ID;

// Pin definitions
extern const int RPM_SENSOR; // Hall-effect or rpm sensor pin
extern const int motorPWM;     // Motor Control PWM pin
extern const int Monitor_LED;  // Built in LED
extern bool Monitor_LED_State;

// Wifi Config [Must match the main fan network AP credentials]
extern const char* ssid;           // WiFi network name
extern const char* password;       // WiFi password

// WebSocket server details [must match main AP]
extern const char* websocket_server;    // Replace with your WebSocket server IP
extern const int websocket_port;                   // Replace with your WebSocket server port
extern const char* websocket_path;              // Replace with your WebSocket path

// PWM Settings
extern const int freq;        // Frequency [Hz]
extern const int resolution;  // PWM resolution

// RPM Measurement
extern const int tRPMDebounceTime;      // [µs] Debounce time for RPM_sensor 
extern const int RPMTimerPrescaler;     // Prescaler value to divide 80 Mhz PLL clock [ensure no decimal for precise timing] [<41.67ms based on 1440rpm (60/1440)]
extern const int RPM_MEASUREMENT_TIME;  // Desired timer frequency at which to calculate the fan rpm [ensure no decimal for precise timing]
extern const int tReportInterval;       // [ms] Approximate interval at which RPM is reported to the fan blade
extern unsigned long int tReport;    // Keeps track of last reporting time

void LED_Blink(); // Toggles built in LED for debugging
void setFanSpeed(uint8_t speed); // Used to set fan speed
void shutDownSystem();  // Shut
