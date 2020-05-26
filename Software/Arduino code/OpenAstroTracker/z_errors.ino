// Error page

#if !defined(__AVR_ATmega2560__) && !defined(ESP8266) && !defined(__AVR_ATmega328P__)
#error "Please select a supported board"
#endif

#if defined TFTscreen && (__AVR_ATmega328P__)
#error "Arduino Uno is not compatible with TFT screens!"
#endif
