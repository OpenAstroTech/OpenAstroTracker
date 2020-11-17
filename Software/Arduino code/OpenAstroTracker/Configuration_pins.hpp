#pragma once

#include "Configuration.hpp"

////////////////////////////////////////////////////////////////////
// Arduino MEGA 
////////////////////////////////////////////////////////////////////
#ifdef __AVR_ATmega2560__  // Arduino Mega
    //---------------------------------------
    // If using 28BYJ-48 and ULN2003 drivers:
    #define RA_IN1_PIN  22
    #define RA_IN2_PIN  24
    #define RA_IN3_PIN  26
    #define RA_IN4_PIN  28
    #define DEC_IN1_PIN 30
    #define DEC_IN2_PIN 32
    #define DEC_IN3_PIN 34
    #define DEC_IN4_PIN 36
    //-----------------------
    // If using NEMA steppers
    #define RA_STEP_PIN 22  // STEP
    #define RA_DIR_PIN  24  // DIR
    #define RA_EN_PIN   26  // Enable
    #define RA_DIAG_PIN 28  // only needed for autohome function
    #define RA_MS0_PIN  30
    #define RA_MS1_PIN  32
    #define RA_MS2_PIN  34
    //RA TMC2209 UART specific pins
    #if UART_SOFTWARESERIAL == 0 //Required for some boards - not needed when using Serial1, Serial2, or Serial3
        #define RA_SERIAL_PORT Serial3  // HardwareSerial port, wire to TX3 for write-only
    #else
        #define RA_SERIAL_PORT_TX 40 // SoftwareSerial TX port
        #define RA_SERIAL_PORT_RX 63 // SoftwareSerial RX port
    #endif
    #define RA_DRIVER_ADDRESS 0b00  // Set by MS1/MS2. LOW/LOW in this case

    #define DEC_STEP_PIN 23  // STEP
    #define DEC_DIR_PIN  25  // DIR
    #define DEC_EN_PIN   27  // Enable
    #define DEC_DIAG_PIN 29  // only needed for autohome function
    #define DEC_MS0_PIN  31
    #define DEC_MS1_PIN  33
    #define DEC_MS2_PIN  35
    //DEC TMC2209 UART specific pins
    #if UART_SOFTWARESERIAL == 0 //Required for some boards - not needed when using Serial1, Serial2, or Serial3
        #define DEC_SERIAL_PORT Serial3  // HardwareSerial port, wire to TX3 for write-only
    #else
        #define DEC_SERIAL_PORT_TX 59 // SoftwareSerial TX port
        #define DEC_SERIAL_PORT_RX 64 // SoftwareSerial RX port
    #endif
    #define DEC_DRIVER_ADDRESS 0b01  // Set by MS1/MS2 (MS1 HIGH, MS2 LOW)
    
    #if AZIMUTH_ALTITUDE_MOTORS == 1
        // Azimuth and Altitude pins
        #define ALT_IN1_PIN 46
        #define ALT_IN2_PIN 48
        #define ALT_IN3_PIN 50
        #define ALT_IN4_PIN 52
        #define AZ_IN1_PIN 47
        #define AZ_IN2_PIN 49
        #define AZ_IN3_PIN 51
        #define AZ_IN4_PIN 53
    #endif
    //---------------
    // MISC PINS amd SETTINGS
    #if USE_GPS == 1
        #define GPS_SERIAL_PORT Serial1
        #define GPS_BAUD_RATE 9600
    #endif
    
    //LCD pin definitions (to allow configuration on different boards)
    #if DISPLAY_TYPE == DISPLAY_TYPE_LCD_KEYPAD
        #define LCD_PINA0 0
        #define LCD_PIN4 4
        #define LCD_PIN5 5
        #define LCD_PIN6 6
        #define LCD_PIN7 7
        #define LCD_PIN8 8
        #define LCD_PIN9 9
    #endif
    
#endif //mega

////////////////////////////////////////////////////////////////////
// ESP32
////////////////////////////////////////////////////////////////////
#ifdef ESP32

  // RA Motor pins
  #define RA_IN1_PIN  19  
  #define RA_IN2_PIN  21  
  #define RA_IN3_PIN  22  
  #define RA_IN4_PIN  23  

  // DEC Motor pins
  #define DEC_IN1_PIN  16  
  #define DEC_IN2_PIN  17  
  #define DEC_IN3_PIN  5   
  #define DEC_IN4_PIN  18  

  // If using NEMA steppers
  #define RA_STEP_PIN 19  // STEP
  #define RA_DIR_PIN  21  // DIR
  #define RA_EN_PIN   22  // Enable
  #define RA_DIAG_PIN 23  // only needed for autohome function
  #define RA_MS0_PIN  4
  #define RA_MS1_PIN  0
  #define RA_MS2_PIN  2

  //RA TMC2209 UART specific pins
  #define RA_SERIAL_PORT Serial2  // HardwareSerial port, wire to TX2 for write-only
  #define RA_DRIVER_ADDRESS 0b00  // Set by MS1/MS2. LOW/LOW in this case

  #define DEC_STEP_PIN 16  // STEP
  #define DEC_DIR_PIN  17  // DIR
  #define DEC_EN_PIN   5   // Enable
  #define DEC_DIAG_PIN 18  // only needed for autohome function
  #define DEC_MS0_PIN  15
  #define DEC_MS1_PIN  8
  #define DEC_MS2_PIN  7

  //DEC TMC2209 UART specific pins
  #define DEC_SERIAL_PORT Serial2  // HardwareSerial port, wire to TX2 for write-only
  #define DEC_DRIVER_ADDRESS 0b01  // Set by MS1/MS2 (MS1 HIGH, MS2 LOW)

    #if AZIMUTH_ALTITUDE_MOTORS == 1
        #error Azimuth/Alt motors not currently configured/supported in ESP32
    #endif 

    #if USE_GPS == 1
        #error GPS module not currently configured/supported in ESP32
    #endif

    #if USE_GYRO_LEVEL == 1
        #error Digital Level not currently configured/supported in ESP32
    #endif
#endif
