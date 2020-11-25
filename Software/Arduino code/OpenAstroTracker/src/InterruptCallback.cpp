#include "InterruptCallback.hpp"
#include "Utility.hpp"

//////////////////////////////////////
// This is an hardware-independent abstraction layer over
// whatever timer is used for the hardware being run
//////////////////////////////////////

#if defined ESP32
  // We don't support ESP32 boards in interrupt mode
#elif defined __AVR_ATmega2560__   // Arduino Mega
  #define USE_TIMER_1     true
  #define USE_TIMER_2     true
  #define USE_TIMER_3     false
  #define USE_TIMER_4     false
  #define USE_TIMER_5     false
  #include "libs/TimerInterrupt/TimerInterrupt.h"
#else
  #error Unrecognized board selected. Either implement interrupt code or define the board here.
#endif

#if defined(ESP32)

/*
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

    LOGV1(DEBUG_INFO, F("Setup ESP32 Timer"));

  return true;
}

void InterruptCallback::stop(){
    LOGV1(DEBUG_INFO, F("Stop ESP32 Timer"));
    if (timerAlarmEnabled(_timer)) {
        timerAlarmDisable(_timer);
    }
}

void InterruptCallback::start(){
    LOGV1(DEBUG_INFO, F("Start ESP32 Timer"));
    if (!timerAlarmEnabled(_timer)){
        timerAlarmEnable(_timer);
    }
}
*/

#elif defined __AVR_ATmega2560__

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
