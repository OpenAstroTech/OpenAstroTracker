/****************************************************************************************************************************
   ESP8266_ISR_Timer.h
   For ESP8266 boards
   Written by Khoi Hoang

   Built by Khoi Hoang https://github.com/khoih-prog/ESP8266TimerInterrupt
   Licensed under MIT license
   Version: 1.0.3

   The ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler. They're only better than UNO / Mega.
   The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.
   The timer1's 23-bit counter terribly can count only up to 8,388,607. So the timer1 maximum interval is very short.
   Using 256 prescaler, maximum timer1 interval is only 26.843542 seconds !!!

   Now with these new 16 ISR-based timers, the maximum interval is practically unlimited (limited only by unsigned long miliseconds)
   The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
   Therefore, their executions are not blocked by bad-behaving functions / tasks.
   This important feature is absolutely necessary for mission-critical tasks.

   Based on SimpleTimer - A timer library for Arduino.
   Author: mromani@ottotecnica.com
   Copyright (c) 2010 OTTOTECNICA Italy

   Based on BlynkTimer.h
   Author: Volodymyr Shymanskyy

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
    1.0.0   K Hoang      23/11/2019 Initial coding
    1.0.1   K Hoang      25/11/2019 New release fixing compiler error
    1.0.2   K.Hoang      26/11/2019 Permit up to 16 super-long-time, super-accurate ISR-based timers to avoid being blocked
    1.0.3   K.Hoang      17/05/2020 Restructure code. Fix example. Enhance README.
*****************************************************************************************************************************/


#ifndef ESP8266_ISR_TIMER_H
#define ESP8266_ISR_TIMER_H

#if !defined(ESP8266)
#error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

#include <stddef.h>
#ifdef ESP8266
extern "C"
{
#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
}
#else
#include <inttypes.h>
#endif

#if defined(ARDUINO)
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#endif

#define ESP8266_ISR_Timer ISRTimer

typedef void (*timer_callback)(void);
typedef void (*timer_callback_p)(void *);

class ESP8266_ISR_Timer 
{

  public:
    // maximum number of timers
    const static int MAX_TIMERS = 16;

    // setTimer() constants
    const static int RUN_FOREVER = 0;
    const static int RUN_ONCE = 1;

    // constructor
    ESP8266_ISR_Timer();

    void ICACHE_RAM_ATTR init();

    // this function must be called inside loop()
    void ICACHE_RAM_ATTR run();

    // Timer will call function 'f' every 'd' milliseconds forever
    // returns the timer number (numTimer) on success or
    // -1 on failure (f == NULL) or no free timers
    int ICACHE_RAM_ATTR setInterval(unsigned long d, timer_callback f);

    // Timer will call function 'f' with parameter 'p' every 'd' milliseconds forever
    // returns the timer number (numTimer) on success or
    // -1 on failure (f == NULL) or no free timers
    int ICACHE_RAM_ATTR setInterval(unsigned long d, timer_callback_p f, void* p);

    // Timer will call function 'f' after 'd' milliseconds one time
    // returns the timer number (numTimer) on success or
    // -1 on failure (f == NULL) or no free timers
    int ICACHE_RAM_ATTR setTimeout(unsigned long d, timer_callback f);

    // Timer will call function 'f' with parameter 'p' after 'd' milliseconds one time
    // returns the timer number (numTimer) on success or
    // -1 on failure (f == NULL) or no free timers
    int ICACHE_RAM_ATTR setTimeout(unsigned long d, timer_callback_p f, void* p);

    // Timer will call function 'f' every 'd' milliseconds 'n' times
    // returns the timer number (numTimer) on success or
    // -1 on failure (f == NULL) or no free timers
    int ICACHE_RAM_ATTR setTimer(unsigned long d, timer_callback f, unsigned n);

    // Timer will call function 'f' with parameter 'p' every 'd' milliseconds 'n' times
    // returns the timer number (numTimer) on success or
    // -1 on failure (f == NULL) or no free timers
    int ICACHE_RAM_ATTR setTimer(unsigned long d, timer_callback_p f, void* p, unsigned n);

    // updates interval of the specified timer
    bool ICACHE_RAM_ATTR changeInterval(unsigned numTimer, unsigned long d);

    // destroy the specified timer
    void ICACHE_RAM_ATTR deleteTimer(unsigned numTimer);

    // restart the specified timer
    void ICACHE_RAM_ATTR restartTimer(unsigned numTimer);

    // returns true if the specified timer is enabled
    bool ICACHE_RAM_ATTR isEnabled(unsigned numTimer);

    // enables the specified timer
    void ICACHE_RAM_ATTR enable(unsigned numTimer);

    // disables the specified timer
    void ICACHE_RAM_ATTR disable(unsigned numTimer);

    // enables all timers
    void ICACHE_RAM_ATTR enableAll();

    // disables all timers
    void ICACHE_RAM_ATTR disableAll();

    // enables the specified timer if it's currently disabled,
    // and vice-versa
    void ICACHE_RAM_ATTR toggle(unsigned numTimer);

    // returns the number of used timers
    unsigned ICACHE_RAM_ATTR getNumTimers();

    // returns the number of available timers
    unsigned ICACHE_RAM_ATTR getNumAvailableTimers() 
    {
      return MAX_TIMERS - numTimers;
    };

  private:
    // deferred call constants
    const static int DEFCALL_DONTRUN = 0;       // don't call the callback function
    const static int DEFCALL_RUNONLY = 1;       // call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;     // call the callback function and delete the timer

    // low level function to initialize and enable a new timer
    // returns the timer number (numTimer) on success or
    // -1 on failure (f == NULL) or no free timers
    int ICACHE_RAM_ATTR setupTimer(unsigned long d, void* f, void* p, bool h, unsigned n);

    // find the first available slot
    int ICACHE_RAM_ATTR findFirstFreeSlot();

    typedef struct 
    {
      unsigned long prev_millis;        // value returned by the millis() function in the previous run() call
      void* callback;                   // pointer to the callback function
      void* param;                      // function parameter
      bool hasParam;                 // true if callback takes a parameter
      unsigned long delay;              // delay value
      unsigned maxNumRuns;              // number of runs to be executed
      unsigned numRuns;                 // number of executed runs
      bool enabled;                  // true if enabled
      unsigned toBeCalled;              // deferred function call (sort of) - N.B.: only used in run()
    } timer_t;

    volatile timer_t timer[MAX_TIMERS];

    // actual number of timers in use (-1 means uninitialized)
    volatile int numTimers;
};

#include "ESP8266_ISR_Timer-Impl.h"

#endif
