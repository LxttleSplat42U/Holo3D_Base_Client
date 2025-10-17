#pragma once
#include "HardwareSoftwareConfig.h"
//Speed RPM timer and interrupt setup
extern volatile int nbladePasses;                 // Counts the number of blade passes
extern const int tRPMDebounceTime;                // [µs] Debounce to prevent multiple triggers 
extern volatile unsigned long int tRPMInterrupt;  // Keeps track of last interrupt timestamp      
extern volatile bool RPMMeasurementEnabled;       // Keeps track if intterrupt is enabled
extern hw_timer_t *RPMTimer;                      // Timer to use for RPM measurements
extern float volatile sumRPM;                     // Total of all rpms measured
extern volatile int avgRPMCount;                  // Number of rpms measured
extern float volatile avgRPM;                     // Average of all measured RPMs 
extern const int RPMTimerPrescaler;               // Prescaler value to divide 80 Mhz PLL clock
extern const int RPMTimerFreq;                    // Frequency of the timer based on the prescaler set
extern const int RPMTimerCount;                   // ARR counter value to repeat at desired frequency
extern const int RPMConversionFactor;             // Convertion factor used to convert blade passes to an estimated rpm

void setupRPMMeasurement();
void setupRPMTimer();
// void enableRPMMeasurement();
// void disableRPMMeasurement();
// void MeasureRPM();


