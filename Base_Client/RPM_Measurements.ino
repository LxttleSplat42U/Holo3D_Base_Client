#include "RPM_Measurements.h"

hw_timer_t *RPMTimer = NULL;
volatile int nbladePasses = 0;
volatile bool RPMMeasurementEnabled = false; 
float volatile sumRPM = 0;                                                    
volatile int avgRPMCount = 0;                                                 
float volatile avgRPM = 0;                                                    
const int RPMTimerFreq = (80000000/RPMTimerPrescaler);
const int RPMTimerCount = (80000000/RPMTimerPrescaler)/RPM_MEASUREMENT_TIME;
const int RPMConversionFactor = (RPMTimerFreq/RPMTimerCount)*60;  // (Timer interrupt interval) * 60 || RPS (rotations per second -> RPM)
volatile unsigned long int tRPMInterrupt = 0;

void IRAM_ATTR INT_bladePassed(){
  if ( (micros() - tRPMInterrupt) >= tRPMDebounceTime){             // Check if at least more than last debounce time
  tRPMInterrupt = micros();
  nbladePasses = nbladePasses + 1;                                  // Inc number of blade passes [Explicit to avoid deprecated warnings]
  digitalWrite(Monitor_LED, digitalRead(Monitor_LED) ? LOW : HIGH); // Toggle Monitor_LED 
  } 
}

void ARDUINO_ISR_ATTR RPMTimerCalculate(){
    sumRPM = sumRPM + nbladePasses*RPMConversionFactor; // Convert to RPM based on bladepasses
    nbladePasses = 0;                                   // Reset blade counter
    avgRPMCount = avgRPMCount + 1;                      // Increase number rpm calculations
}

void setupRPMTimer(){
  RPMTimer = timerBegin(RPMTimerFreq);

  // Debug serial messages to confirm timer successfull setup
  if (RPMTimer == NULL){
    Serial.println("RPMTimer initialise Error");
  } else{
    timerAttachInterrupt(RPMTimer, &RPMTimerCalculate);             // Attach the interrupt function
    timerAlarm(RPMTimer, RPMTimerCount, true,0);                    // Repeat with unlimited count
    Serial.println("RPMTimer initialised with frequency: " + String(timerGetFrequency(RPMTimer)) + " Hz");
    Serial.println("Repeating every " + String(RPMTimerFreq/RPMTimerCount) + " Hz");
  }  

}

void setupRPMMeasurement(){
   pinMode(RPM_SENSOR, INPUT_PULLUP);                                             // Set pin as input with pull up resistor
   attachInterrupt(digitalPinToInterrupt(RPM_SENSOR), INT_bladePassed, FALLING);  // Attach interrupt function on falling edge
   RPMMeasurementEnabled = true;
   pinMode(D13, OUTPUT);                                                          // Debugging link Builtin LED to interrupt
   digitalWrite(D13, LOW);
}