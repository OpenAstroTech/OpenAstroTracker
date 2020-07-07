#pragma once

#include "Globals.hpp"

//////////////////////////////////////
// This is an hardware-independent abstraction layer over 
// whatever timer is used for the hardware being run.
//////////////////////////////////////

// The callback function signature
typedef void (*interrupt_callback_p)(void*);

// The static class managing the callbacks.
class InterruptCallback
{
public:
  // Requests the hardware to call the given callback with the given payload at the given interval in milliseconds.
  // The interrupts should be started before returning.
  bool static setInterval(float intervalMs, interrupt_callback_p callback, void* payload);

  // Starts the timer interrupts (currently not called/used)
  void static start();

  // Stops the timer interrupts (currently not called/used)
  void static stop();
};

