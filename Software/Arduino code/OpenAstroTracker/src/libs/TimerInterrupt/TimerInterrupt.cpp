/****************************************************************************************************************************
   TimerInterrupt.cpp
   For Arduino AVR boards
   Written by Khoi Hoang

   Built by Khoi Hoang https://github.com/khoih-prog/TimerInterrupt
   Licensed under MIT license
   Version: 1.0.2

   TCNTx - Timer/Counter Register. The actual timer value is stored here.
   OCRx - Output Compare Register
   ICRx - Input Capture Register (only for 16bit timer)
   TIMSKx - Timer/Counter Interrupt Mask Register. To enable/disable timer interrupts.
   TIFRx - Timer/Counter Interrupt Flag Register. Indicates a pending timer interrupt.

   Now with we can use these new 16 ISR-based timers, while consuming only 1 hwarware Timer.
   Their independently-selected, maximum interval is practically unlimited (limited only by unsigned long miliseconds)
   The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
   Therefore, their executions are not blocked by bad-behaving functions / tasks.
   This important feature is absolutely necessary for mission-critical tasks.

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
    1.0.0   K Hoang      13/11/2019 Initial coding
    1.0.1   K Hoang      16/11/2019 Add long timer feature, clean up, higher accuracy
    1.0.2   K Hoang      28/11/2019 Permit up to 16 super-long-time, super-accurate ISR-based timers to avoid being blocked
****************************************************************************************************************************/
#if !defined(ESP32)

#define TIMER_INTERRUPT_DEBUG      0
#include "TimerInterrupt.h"

//#ifndef TIMER_INTERRUPT_DEBUG
#define TIMER_INTERRUPT_DEBUG      0
//#endif

void TimerInterrupt::init(int8_t timer)
{
  // Set timer specific stuff
  // All timers in CTC mode
  // 8 bit timers will require changing prescalar values,
  // whereas 16 bit timers are set to either ck/1 or ck/64 prescalar

  //cli();//stop interrupts
  noInterrupts();

  switch (timer)
  {
#if defined(TCCR1A) && defined(TCCR1B) && defined(WGM12)
    case 1:
      // 16 bit timer
      TCCR1A = 0;
      TCCR1B = 0;
      // Page 172-173. ATmega 328/328P or Page 145-146 of ATmega 640/1280/2560
      // Mode 4 => Clear Timer on Compare match (CTC) using OCR1A for counter value
      bitWrite(TCCR1B, WGM12, 1);
      // No scaling now
      bitWrite(TCCR1B, CS10, 1);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("T1");
#endif

      break;
#endif

#if defined(TCCR2A) && defined(TCCR2B)
    case 2:
      // 8 bit timer
      TCCR2A = 0;
      TCCR2B = 0;
      // Page 205-206. ATmegal328, Page 184-185 ATmega 640/1280/2560
      // Mode 2 => Clear Timer on Compare match (CTC) using OCR2A for counter value
      bitWrite(TCCR2A, WGM21, 1);
      // No scaling now
      bitWrite(TCCR2B, CS20, 1);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("T2");
#endif

      break;
#endif

#if defined(TCCR3A) && defined(TCCR3B) &&  defined(TIMSK3)
    case 3:
      // 16 bit timer
      TCCR3A = 0;
      TCCR3B = 0;
      bitWrite(TCCR3B, WGM32, 1);
      bitWrite(TCCR3B, CS30, 1);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("T3");
#endif

      break;
#endif

#if defined(TCCR4A) && defined(TCCR4B) &&  defined(TIMSK4)
    case 4:
      // 16 bit timer
      TCCR4A = 0;
      TCCR4B = 0;
#if defined(WGM42)
      bitWrite(TCCR4B, WGM42, 1);
#elif defined(CS43)
      // TODO this may not be correct
      // atmega32u4
      bitWrite(TCCR4B, CS43, 1);
#endif
      bitWrite(TCCR4B, CS40, 1);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("T4");
#endif

      break;
#endif

#if defined(TCCR5A) && defined(TCCR5B) &&  defined(TIMSK5)
    case 5:
      // 16 bit timer
      TCCR5A = 0;
      TCCR5B = 0;
      bitWrite(TCCR5B, WGM52, 1);
      bitWrite(TCCR5B, CS50, 1);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("T5");
#endif

      break;
#endif
  }

  _timer = timer;

  //sei();//enable interrupts
  interrupts();

}

void TimerInterrupt::set_OCR()
{
  // Run with noInterrupt()
  // Set the OCR for the given timer,
  // set the toggle count,
  // then turn on the interrupts
  uint32_t _OCRValueToUse;

#if TIMER_INTERRUPT_DEBUG > 2
  Serial.println("set_OCR: start with " + String(_timer));
#endif


  switch (_timer)
  {
    case 1:
      _OCRValueToUse = min(MAX_COUNT_16BIT, _OCRValueRemaining);
      OCR1A = _OCRValueToUse;
      _OCRValueRemaining -= _OCRValueToUse;

#if defined(OCR1A) && defined(TIMSK1) && defined(OCIE1A)
      // Bit 1 – OCIEA: Output Compare A Match Interrupt Enable
      // When this bit is written to '1', and the I-flag in the Status Register is set (interrupts globally enabled), the
      // Timer/Counter Output Compare A Match interrupt is enabled. The corresponding Interrupt Vector is
      // executed when the OCFA Flag, located in TIFR1, is set.
      bitWrite(TIMSK1, OCIE1A, 1);
#elif defined(OCR1A) && defined(TIMSK) && defined(OCIE1A)
      // this combination is for at least the ATmega32
      bitWrite(TIMSK, OCIE1A, 1);
#endif
      break;

#if defined(OCR2A) && defined(TIMSK2) && defined(OCIE2A)
    case 2:
      _OCRValueToUse = min(MAX_COUNT_8BIT, _OCRValueRemaining);
      OCR2A = _OCRValueToUse;
      _OCRValueRemaining -= _OCRValueToUse;

      bitWrite(TIMSK2, OCIE2A, 1);
#if TIMER_INTERRUPT_DEBUG > 2
      Serial.println("set_OCR: case 2 " + String(_OCRValueToUse)+" "+String(_OCRValueRemaining));
#endif
      break;
#endif

#if defined(OCR3A) && defined(TIMSK3) && defined(OCIE3A)
    case 3:
      _OCRValueToUse = min(MAX_COUNT_16BIT, _OCRValueRemaining);
      OCR3A = _OCRValueToUse;
      _OCRValueRemaining -= _OCRValueToUse;

      bitWrite(TIMSK3, OCIE3A, 1);
      break;
#endif

#if defined(OCR4A) && defined(TIMSK4) && defined(OCIE4A)
    case 4:
      _OCRValueToUse = min(MAX_COUNT_16BIT, _OCRValueRemaining);
      OCR4A = _OCRValueToUse;
      _OCRValueRemaining -= _OCRValueToUse;

      bitWrite(TIMSK4, OCIE4A, 1);
      break;
#endif

#if defined(OCR5A) && defined(TIMSK5) && defined(OCIE5A)
    case 5:
      _OCRValueToUse = min(MAX_COUNT_16BIT, _OCRValueRemaining);
      OCR5A = _OCRValueToUse;
      _OCRValueRemaining -= _OCRValueToUse;

      bitWrite(TIMSK5, OCIE5A, 1);
      break;
#endif
  }

  // Flag _OCRValue == 0 => end of long timer
  if (_OCRValueRemaining == 0)
    _timerDone = true;

}

// frequency (in hertz) and duration (in milliseconds).
// Return true if frequency is OK with selected timer (OCRValue is in range)
bool TimerInterrupt::setFrequency(float frequency, timer_callback_p callback, uint32_t params, unsigned long duration)
{
#if (TIMER_INTERRUPT_DEBUG > 2)
  Serial.println("setFrequency: called (" + String(frequency, 3) + ")");
#endif

  uint8_t       andMask = 0b11111000;
  unsigned long OCRValue;
  bool isSuccess = false;

  //frequencyLimit must > 1
  float frequencyLimit = frequency * 17179.840;

  #if (TIMER_INTERRUPT_DEBUG > 2)
  Serial.println("setFreq: limit is " + String(frequencyLimit, 1));
  #endif

  // Limit frequency to larger than (0.00372529 / 64) Hz or interval 17179.840s / 17179840 ms to avoid uint32_t overflow
  if ((_timer <= 0) || (callback == NULL) || ((frequencyLimit) < 1) )
  {
#if (TIMER_INTERRUPT_DEBUG > 2)
    Serial.println("setFreq: limit out of range!");
#endif
    return false;
  }
  else
  {
    // Calculate the toggle count. Duration must be at least longer then one cycle
    if (duration > 0)
    {
      _toggle_count = frequency * duration / 1000;

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("setFrequency => _toggle_count = " + String(_toggle_count) + ", frequency = " + String(frequency) + ", duration = " + String(duration));
#endif

      if (_toggle_count < 1)
      {
        return false;
      }
    }
    else
    {
      _toggle_count = -1;
    }

    //Timer0 and timer2 are 8 bit timers, meaning they can store a maximum counter value of 255.
    //Timer2 does not have the option of 1024 prescaler, only 1, 8, 32, 64
    //Timer1 is a 16 bit timer, meaning it can store a maximum counter value of 65535.
    int prescalerIndexStart;

    //Use smallest prescaler first, then increase until fits (<255)
    if (_timer != 2)
    {
      if (frequencyLimit > 64)
        prescalerIndexStart = NO_PRESCALER;
      else if (frequencyLimit > 8)
        prescalerIndexStart = PRESCALER_8;
      else
        prescalerIndexStart = PRESCALER_64;


      for (int prescalerIndex = prescalerIndexStart; prescalerIndex <= PRESCALER_1024; prescalerIndex++)
      {
        OCRValue = F_CPU / (frequency * prescalerDiv[prescalerIndex]) - 1;

#if (TIMER_INTERRUPT_DEBUG > 0)
        Serial.println("Freq * 1000 = " + String(frequency * 1000));
        Serial.println("F_CPU = " + String(F_CPU) + ", preScalerDiv = " + String(prescalerDiv[prescalerIndex]));
        Serial.println("OCR = " + String(OCRValue) + ", preScalerIndex = " + String(prescalerIndex));
#endif

        // We use very large _OCRValue now, and every time timer ISR activates, we deduct min(MAX_COUNT_16BIT, _OCRValueRemaining) from _OCRValueRemaining
        // So that we can create very long timer, even if the counter is only 16-bit.
        // Use very high frequency (OCRValue / MAX_COUNT_16BIT) around 64 * 1024 to achieve higher accuracy
        if ( (OCRValue / MAX_COUNT_16BIT) < 16384 )
        {
          _OCRValue           = OCRValue;
          _OCRValueRemaining  = OCRValue;
          _prescalerIndex = prescalerIndex;

#if (TIMER_INTERRUPT_DEBUG > 0)
          Serial.println("OK in loop => _OCR = " + String(_OCRValue) + ", _preScalerIndex = " + String(_prescalerIndex) + ", preScalerDiv = " + String(prescalerDiv[_prescalerIndex]));
#endif

          isSuccess = true;

          break;
        }
      }

      if (!isSuccess)
      {
        // Always do this
        _OCRValue           = OCRValue;
        _OCRValueRemaining  = OCRValue;
        _prescalerIndex = PRESCALER_1024;

#if (TIMER_INTERRUPT_DEBUG > 0)
        Serial.println("OK out loop => _OCR = " + String(_OCRValue) + ", _preScalerIndex = " + String(_prescalerIndex) + ", preScalerDiv = " + String(prescalerDiv[_prescalerIndex]));
#endif
      }
    }
    else
    {
      if (frequencyLimit > 64)
        prescalerIndexStart = T2_NO_PRESCALER;
      else if (frequencyLimit > 8)
        prescalerIndexStart = T2_PRESCALER_8;
      else if (frequencyLimit > 2)
        prescalerIndexStart = T2_PRESCALER_32;
      else
        prescalerIndexStart = T2_PRESCALER_64;

      // Page 206-207. ATmegal328
      //8-bit Timer2 has more options up to 1024 prescaler, from 1, 8, 32, 64, 128, 256 and 1024
      for (int prescalerIndex = prescalerIndexStart; prescalerIndex <= T2_PRESCALER_1024; prescalerIndex++)
      {
        OCRValue = F_CPU / (frequency * prescalerDivT2[prescalerIndex]) - 1;

#if (TIMER_INTERRUPT_DEBUG > 0)
        Serial.println("F_CPU = " + String(F_CPU) + ", preScalerDiv = " + String(prescalerDivT2[prescalerIndex]));
        Serial.println("OCR2 = " + String(OCRValue) + ", preScalerIndex = " + String(prescalerIndex));
#endif

        // We use very large _OCRValue now, and every time timer ISR activates, we deduct min(MAX_COUNT_8BIT, _OCRValue) from _OCRValue
        // to create very long timer, even if the counter is only 16-bit.
        // Use very high frequency (OCRValue / MAX_COUNT_16BIT) around 64 * 1024 to achieve higher accuracy
        if ( (OCRValue / MAX_COUNT_8BIT) < 16384 )
        {
          _OCRValue           = OCRValue;
          _OCRValueRemaining  = OCRValue;
          // same as prescalarbits
          _prescalerIndex = prescalerIndex;

#if (TIMER_INTERRUPT_DEBUG > 0)
          Serial.println("OK in loop => _OCR = " + String(_OCRValue) + ", _preScalerIndex = " + String(_prescalerIndex) + ", preScalerDiv = " + String(prescalerDivT2[_prescalerIndex]));
#endif

          isSuccess = true;

          break;
        }
      }

      if (!isSuccess)
      {
        // Always do this
        _OCRValue           = OCRValue;
        _OCRValueRemaining  = OCRValue;
        // same as prescalarbits
        _prescalerIndex = T2_PRESCALER_1024;

#if (TIMER_INTERRUPT_DEBUG > 0)
        Serial.println("OK out loop => _OCR = " + String(_OCRValue) + ", _preScalerIndex = " + String(_prescalerIndex) + ", preScalerDiv = " + String(prescalerDivT2[_prescalerIndex]));
#endif
      }
    }

    //cli();//stop interrupts
    noInterrupts();

    _frequency = frequency;
    _callback  = (void*) callback;
    _params    = reinterpret_cast<void*>(params);

    _timerDone = false;

    // 8 bit timers from here
#if defined(TCCR2B)
    if (_timer == 2)
    {
      TCCR2B = (TCCR2B & andMask) | _prescalerIndex;   //prescalarbits;
#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("TCCR2B = " + String(TCCR2B));
#endif
    }
#endif

    // 16 bit timers from here
#if defined(TCCR1B)
    else if (_timer == 1)
    {
      TCCR1B = (TCCR1B & andMask) | _prescalerIndex;   //prescalarbits;
#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("TCCR1B = " + String(TCCR1B));
#endif

    }
#endif

#if defined(TCCR3B)
    else if (_timer == 3)
      TCCR3B = (TCCR3B & andMask) | _prescalerIndex;   //prescalarbits;
#endif

#if defined(TCCR4B)
    else if (_timer == 4)
      TCCR4B = (TCCR4B & andMask) | _prescalerIndex;   //prescalarbits;
#endif

#if defined(TCCR5B)
    else if (_timer == 5)
      TCCR5B = (TCCR5B & andMask) | _prescalerIndex;   //prescalarbits;
#endif

    // Set the OCR for the given timer,
    // set the toggle count,
    // then turn on the interrupts
    set_OCR();

#if TIMER_INTERRUPT_DEBUG > 2
    Serial.println("setfreq: set Ocr returned . Enabling interrupts.");
#endif

    //sei();//allow interrupts
    interrupts();


    return true;
  }
}

void TimerInterrupt::detachInterrupt(void)
{
  //cli();//stop interrupts
  noInterrupts();

  switch (_timer)
  {
#if defined(TIMSK1) && defined(OCIE1A)
    case 1:
      bitWrite(TIMSK1, OCIE1A, 0);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Disable T1");
#endif

      break;
#endif

    case 2:
#if defined(TIMSK2) && defined(OCIE2A)
      bitWrite(TIMSK2, OCIE2A, 0); // disable interrupt
#endif

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Disable T2");
#endif

      break;

#if defined(TIMSK3) && defined(OCIE3A)
    case 3:
      bitWrite(TIMSK3, OCIE3A, 0);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Disable T3");
#endif

      break;
#endif

#if defined(TIMSK4) && defined(OCIE4A)
    case 4:
      bitWrite(TIMSK4, OCIE4A, 0);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Disable T4");
#endif

      break;
#endif

#if defined(TIMSK5) && defined(OCIE5A)
    case 5:
      bitWrite(TIMSK5, OCIE5A, 0);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Disable T5");
#endif

      break;
#endif
  }

  //sei();//allow interrupts
  interrupts();
}

// Duration (in milliseconds). Duration = 0 or not specified => run indefinitely
void TimerInterrupt::reattachInterrupt(unsigned long duration)
{
  //cli();//stop interrupts
  noInterrupts();

  // Calculate the toggle count
  if (duration > 0)
  {
    _toggle_count = _frequency * duration / 1000;
  }
  else
  {
    _toggle_count = -1;
  }

  switch (_timer)
  {
#if defined(TIMSK1) && defined(OCIE1A)
    case 1:
      bitWrite(TIMSK1, OCIE1A, 1);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Enable T1");
#endif

      break;
#endif

    case 2:
#if defined(TIMSK2) && defined(OCIE2A)
      bitWrite(TIMSK2, OCIE2A, 1); // enable interrupt
#endif

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Enable T2");
#endif

      break;

#if defined(TIMSK3) && defined(OCIE3A)
    case 3:
      bitWrite(TIMSK3, OCIE3A, 1);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Enable T3");
#endif

      break;
#endif

#if defined(TIMSK4) && defined(OCIE4A)
    case 4:
      bitWrite(TIMSK4, OCIE4A, 1);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Enable T4");
#endif

      break;
#endif

#if defined(TIMSK5) && defined(OCIE5A)
    case 5:
      bitWrite(TIMSK5, OCIE5A, 1);

#if (TIMER_INTERRUPT_DEBUG > 0)
      Serial.println("Enable T5");
#endif

      break;
#endif
  }

  //sei();//allow interrupts
  interrupts();
}

// Just stop clock source, still keep the count
void TimerInterrupt::pauseTimer(void)
{
  uint8_t andMask = 0b11111000;

  //Just clear the CSx2-CSx0. Still keep the count in TCNT and Timer Interrupt mask TIMKSx.

  // 8 bit timers from here
#if defined(TCCR2B)
  if (_timer == 2)
  {
    TCCR2B = (TCCR2B & andMask);
#if (TIMER_INTERRUPT_DEBUG > 0)
    Serial.println("TCCR2B = " + String(TCCR2B));
#endif
  }
#endif

  // 16 bit timers from here
#if defined(TCCR1B)
  else if (_timer == 1)
  {
    TCCR1B = (TCCR1B & andMask);
#if (TIMER_INTERRUPT_DEBUG > 0)
    Serial.println("TCCR1B = " + String(TCCR1B));
#endif
  }
#endif

#if defined(TCCR3B)
  else if (_timer == 3)
    TCCR3B = (TCCR3B & andMask);
#endif

#if defined(TCCR4B)
  else if (_timer == 4)
    TCCR4B = (TCCR4B & andMask);
#endif

#if defined(TCCR5B)
  else if (_timer == 5)
    TCCR5B = (TCCR5B & andMask);
#endif
}

// Just reconnect clock source, continue from the current count
void TimerInterrupt::resumeTimer(void)
{
  uint8_t andMask = 0b11111000;

  //Just restore the CSx2-CSx0 stored in _prescalerIndex. Still keep the count in TCNT and Timer Interrupt mask TIMKSx.
  // 8 bit timers from here
#if defined(TCCR2B)
  if (_timer == 2)
  {
    TCCR2B = (TCCR2B & andMask) | _prescalerIndex;   //prescalarbits;
#if (TIMER_INTERRUPT_DEBUG > 0)
    Serial.println("TCCR2B = " + String(TCCR2B));
#endif
  }
#endif

  // 16 bit timers from here
#if defined(TCCR1B)
  else if (_timer == 1)
  {
    TCCR1B = (TCCR1B & andMask) | _prescalerIndex;   //prescalarbits;
#if (TIMER_INTERRUPT_DEBUG > 0)
    Serial.println("TCCR1B = " + String(TCCR1B));
#endif
  }
#endif

#if defined(TCCR3B)
  else if (_timer == 3)
    TCCR3B = (TCCR3B & andMask) | _prescalerIndex;   //prescalarbits;
#endif

#if defined(TCCR4B)
  else if (_timer == 4)
    TCCR4B = (TCCR4B & andMask) | _prescalerIndex;   //prescalarbits;
#endif

#if defined(TCCR5B)
  else if (_timer == 5)
    TCCR5B = (TCCR5B & andMask) | _prescalerIndex;   //prescalarbits;
#endif
}

#endif