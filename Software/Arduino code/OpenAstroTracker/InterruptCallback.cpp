#include "InterruptCallback.hpp"
#include "Utility.hpp"

//////////////////////////////////////
// This is an hardware-independent abstraction layer over
// whatever timer is used for the hardware being run
//////////////////////////////////////

// NOTE: ESP8266 support is not complete and does not work. This code is never called.
#ifdef ESP8266
  #include "ESP8266TimerInterrupt.h"
#elif defined ESP32
  // We don't support ESP32 boards in interrupt mode
#elif defined __AVR_ATmega328P__ || defined __AVR_ATmega2560__   // Arduino Uno or Mega
  #define USE_TIMER_1     true
  #define USE_TIMER_2     true
  #define USE_TIMER_3     false
  #define USE_TIMER_4     false
  #define USE_TIMER_5     false
  #include "TimerInterrupt.h"
#else
  // Need to add support for ESP32 boards here

  #error Only Arduino Uno, Mega and ESP8266 boards currently supported.
#endif

#ifdef ESP8266
ESP8266Timer interruptHandler;

void* actualPayload = NULL;
volatile interrupt_callback_p actualCallback = NULL;

// This timer only supports a callback with no payload
void ICACHE_RAM_ATTR esp8266callback(void)
{
  if (actualCallback != NULL) {
    (*actualCallback)(actualPayload);
  }
}

bool InterruptCallback::setInterval(float intervalMs, interrupt_callback_p callback, void* payload)
{
  // Since this timer only supports a callback with no payload, we store the requested callback and payload
  // in static variables and install an intermediate callback for the interrupt to call. The intermerdiate
  // The calls the actual requested callback with the payload.
  actualPayload = payload;
  actualCallback = callback;

  // This timer library requires microsecond interval definitions
  interruptHandler.setInterval(1000.0f * intervalMs, esp8266callback);
}

void InterruptCallback::start()
{
}

void InterruptCallback::stop()
{
}

#elif defined(ESP32)

volatile bool _lock = false;

void IRAM_ATTR callbackProxy() {
    if (_lock) {
        return;
    }

    timerAlarmDisable(_timer);
    _lock = true;
    if (func != NULL) {
        func(data);
    }
    _lock = false;
    timerAlarmEnable(_timer);
}

bool InterruptCallback::setInterval(float intervalMs, interrupt_callback_p callback, void* payload){
    func = callback;
    data = payload;

    // 80 divisor = 1uS resolution.
    _timer = timerBegin(0, 80, true);
    timerAttachInterrupt(_timer, callbackProxy, true);
    timerAlarmWrite(_timer, (uint64_t)(intervalMs * 1000.0f), true);
    timerAlarmEnable(_timer);

#ifdef DEBUG_MODE
    Serial.println("Setup ESP32 Timer");
#endif // DEBUG_MODE

}

void InterruptCallback::stop(){
#ifdef DEBUG_MODE
    Serial.println("Stop ESP32 Timer");
#endif // DEBUG_MODE
    if (timerAlarmEnabled(_timer)) {
        timerAlarmDisable(_timer);
    }
}

void InterruptCallback::start(){
#ifdef DEBUG_MODE
    Serial.println("Start ESP32 Timer");
#endif // DEBUG_MODE
    if (!timerAlarmEnabled(_timer)){
        timerAlarmEnable(_timer);
    }
}


#elif defined __AVR_ATmega328P__ || defined __AVR_ATmega2560__

  bool InterruptCallback::setInterval(float intervalMs, interrupt_callback_p callback, void* payload)
{
  // We have requested to use Timer2 (see above)
  ITimer2.init();

  // This timer supports the callback with payload
  return ITimer2.attachInterruptInterval<void*>(intervalMs, callback, payload, 0UL);
}

void InterruptCallback::stop()
{
  ITimer2.stopTimer();
}

void InterruptCallback::start()
{
  ITimer2.restartTimer();
}

#endif
