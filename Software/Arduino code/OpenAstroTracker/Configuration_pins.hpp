
// Arduino MEGA 
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
#define RA_EN_PIN   25  // Enable
#define RA_MS0_PIN  30, HIGH    // 
#define RA_MS1_PIN  32, HIGH
#define RA_MS2_PIN  34, HIGH
//RA TMC2209 UART specific pins
#define RA_SERIAL_PORT Serial3  // HardwareSerial port, wire to TX3 for write-only
#define RA_DRIVER_ADDRESS 0b00  // Set by MS1/MS2. LOW/LOW in this case
#define RA_DIAG_PIN 40          // only needed for autohome function

#define DEC_STEP_PIN 26  // STEP
#define DEC_DIR_PIN  28  // DIR
#define DEC_EN_PIN   29  // Enable
#define DEC_MS0_PIN  31  // 
#define DEC_MS1_PIN  33
#define DEC_MS2_PIN  35
//DEC TMC2209 UART specific pins
#define DEC_SERIAL_PORT Serial3  // HardwareSerial port, wire to TX2 for write-only
#define DEC_DRIVER_ADDRESS 0b01  // Set by MS1/MS2 (MS1 HIGH, MS2 LOW)
#define DEC_DIAG_PIN 41          // only needed for autohome function
//---------------
// MISC PINS
#define GPS_SERIAL_PORT Serial1
#define GPS_BAUD_RATE 9600
#endif //mega

// ESP32
#ifdef ESP32
// todo
#endif

// Arduino UNO
#ifdef __AVR_ATmega328P__ // Arduino Uno
#define RA_IN1_PIN  12
#define RA_IN2_PIN  11
#define RA_IN3_PIN  3
#define RA_IN4_PIN  2

#define DEC_IN1_PIN 18
#define DEC_IN2_PIN 17
#define DEC_IN3_PIN 16
#define DEC_IN4_PIN 15
#endif
