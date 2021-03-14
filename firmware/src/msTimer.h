// msTimer
// 
// In loop (non-blocking) timer.
//
// Current Version: 1.1
// 
// Version 1.0 : Initial release.
// Version 1.1 : Bug fix t prevent rollover error.

#ifndef MS_TIMER_H
#define MS_TIMER_H

#include <Arduino.h>

// Non-blocking millisecond timer.
class msTimer
{

private:
  unsigned long _startMillis;
  unsigned long _delay;
  bool _forceTriggerFlag;

public:
  // Default Constructor.
  msTimer()
  {
    _startMillis = millis();
    _delay = 1000;
    _forceTriggerFlag = false;  
  }

  // Constructor.
  msTimer(unsigned long delay)
  {
    _startMillis = millis();
    _delay = delay;
    _forceTriggerFlag = false;  
  }

  // Returns true if delay has elapsed.
  // Reset delay.
  inline bool elapsed()
  {
    if (_forceTriggerFlag)
    {
      _forceTriggerFlag = false;
      _startMillis = millis();
      return 1;
    }

    if ((millis() - _startMillis ) > _delay)
    {
      _startMillis = millis();
      return 1;
    }

    return 0;
  }

  inline void ForceTrigger()
  {
   _forceTriggerFlag = true;
  }

  // Set delay and reset timer.
  inline void setDelayAndReset(unsigned long delay)
  {
    _delay = delay;
    _startMillis = millis();
  }

  // Set delay and reset timer if delay is different.
  inline void setDelay(unsigned long delay)
  {
    if (_delay != delay)
    {
      _startMillis = millis();
      _delay = delay;
    }
  }

  // Reset timer.
  inline void resetDelay()
  {
    _startMillis = millis();
  }

  //deconstructor
  //~mstimer() {}
};

#endif