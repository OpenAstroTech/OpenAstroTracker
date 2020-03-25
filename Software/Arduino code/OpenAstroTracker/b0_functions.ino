
int read_LCD_buttons() {
  adc_key_in = analogRead(0);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 600)  return btnLEFT;
  if (adc_key_in < 920)  return btnSELECT;
}

// Adjust the given number by the given adjustment, wrap around the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustWrap(int current, int adjustBy, int minVal, int maxVal)
{
  current += adjustBy;
  if (current > maxVal) current -= (maxVal + 1);
  if (current < minVal) current += (maxVal + 1);
  return current;
}

// Adjust the given number by the given adjustment, clamping to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
int adjustClamp(int current, int adjustBy, int minVal, int maxVal)
{
  current += adjustBy;
  if (current > maxVal) current = maxVal;
  if (current < minVal) current = minVal;
  return current;
}

// Adjust the given seconds by the given adjustment, wrapping around.
int adjustSecond(int current, int adjustBy) {
  return adjustWrap(current, adjustBy, 0, 59);
}

// Adjust the given minutes by the given adjustment, wrapping around.
int adjustMinute(int current, int adjustBy) {
  return adjustWrap(current, adjustBy, 0, 59);
}

// Adjust the given hour by the given adjustment, wrapping around.
int adjustHour(int current, int adjustBy) {
  return adjustWrap(current, adjustBy, 0, 23);
}

// Clamp the given number to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
long clamp(long current, long minVal, long maxVal)
{
  if (current > maxVal) current = maxVal;
  if (current < minVal) current = minVal;
  return current;
}

// Clamp the given number to the limits.
// Limits are inclusive, so they represent the lowest and highest valid number.
float clamp(float current, float minVal, float maxVal)
{
  if (current > maxVal) current = maxVal;
  if (current < minVal) current = minVal;
  return current;
}

#ifdef DEBUG_MODE
void log(const char* input) {
  Serial.println(input);
  Serial.flush();
}

void log(String input) {
  Serial.println(input);
  Serial.flush();
}

void logv(const char* input, ...) {
  va_list argp;
  va_start(argp, input);
  Serial.println(formatArg(input, argp));
  Serial.flush();
  va_end(argp);
  
}
#endif 

String format(const char* input, ...) {
  va_list argp;
  va_start(argp, input);
  String ret = formatArg(input, argp);
  va_end(argp);
  return ret;
}

String formatArg(const char* input, va_list args) {
  char achBuffer[128];
  char*p = achBuffer;

  for (const char* i = input; *i != 0; i++) {
    if (*i != '%') {
      *p++ = *i;
      continue;
    }
    i++;
    switch (*i) {
      case '%': {
          *p++ = '%';
        }
        break;

      case 'c': {
          char *ch = va_arg(args, char*);
          *p++ = *ch;
        }
        break;

      case 's': {
          char* s = va_arg(args, char*);
          strcpy(p, s);
          p += strlen(s);
        }
        break;

      case 'd': {
          String s = String((int)va_arg(args, int));
          strcpy(p, s.c_str());
          p += s.length();
        }
        break;

      case 'l': {
          String s = String((long)va_arg(args, long));
          strcpy(p, s.c_str());
          p += s.length();
        }
        break;

      case 'f': {
          float num = (float)va_arg(args, double);
          String s = String(num, 4);
          strcpy(p, s.c_str());
          p += s.length();
        }
        break;

      default: {
          *p++ = *i;
        }
        break;
    }
  }

  *p = '\0';
  return String(achBuffer);
}
