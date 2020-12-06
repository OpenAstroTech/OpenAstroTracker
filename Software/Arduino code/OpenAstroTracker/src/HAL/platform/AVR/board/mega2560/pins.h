#pragma once

// ULN2003 driver
#define RA_IN1_PIN 22
#define RA_IN2_PIN 24
#define RA_IN3_PIN 26
#define RA_IN4_PIN 28
#define DEC_IN1_PIN 30
#define DEC_IN2_PIN 32
#define DEC_IN3_PIN 34
#define DEC_IN4_PIN 36

// TMC drivers
#define RA_STEP_PIN 22 // STEP
#define RA_DIR_PIN 24  // DIR
#define RA_EN_PIN 26   // Enable
#define RA_DIAG_PIN 28 // only needed for autohome function
#define RA_MS0_PIN 30
#define RA_MS1_PIN 32
#define RA_MS2_PIN 34
// RA TMC2209 UART specific pins
#define RA_SERIAL_PORT_TX 14   // SoftwareSerial TX port
#define RA_SERIAL_PORT_RX 15   // SoftwareSerial RX port
#define RA_DRIVER_ADDRESS 0b00 // Set by MS1/MS2. LOW/LOW in this case

#define DEC_STEP_PIN 23 // STEP
#define DEC_DIR_PIN 25  // DIR
#define DEC_EN_PIN 27   // Enable
#define DEC_DIAG_PIN 29 // only needed for autohome function
#define DEC_MS0_PIN 31
#define DEC_MS1_PIN 33
#define DEC_MS2_PIN 35
// DEC TMC2209 UART specific pins
#define DEC_SERIAL_PORT_TX 14   // SoftwareSerial TX port
#define DEC_SERIAL_PORT_RX 15   // SoftwareSerial RX port
#define DEC_DRIVER_ADDRESS 0b01 // Set by MS1/MS2 (MS1 HIGH, MS2 LOW)

// ALT/AZ
#define AZ_IN1_PIN 47
#define AZ_IN2_PIN 49
#define AZ_IN3_PIN 51
#define AZ_IN4_PIN 53
#define ALT_IN1_PIN 46
#define ALT_IN2_PIN 48
#define ALT_IN3_PIN 50
#define ALT_IN4_PIN 52

// GPS
#define GPS_SERIAL_PORT Serial1
#define GPS_BAUD_RATE 9600

// LCD (KEYPAD)
#define LCD_PINA0 0
#define LCD_PIN4 4
#define LCD_PIN5 5
#define LCD_PIN6 6
#define LCD_PIN7 7
#define LCD_PIN8 8
#define LCD_PIN9 9