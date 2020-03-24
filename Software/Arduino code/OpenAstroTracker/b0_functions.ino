void log(const char* input) {
  Serial.println(input);
  Serial.flush();
}

void logv(const char* input, ...) {
  va_list argp;
  va_start(argp, input);
//  Serial.println("HERE");
  Serial.println(formatArg(input, argp));
  Serial.flush();
  va_end(argp);
  
}

String format(const char* input, ...) {
  va_list argp;
  va_start(argp, input);
  String ret = formatArg(input, argp);
  va_end(argp);
  return ret;
}

String formatArg(const char* input, va_list args) {
  char achBuffer[256];
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
          byte b = (byte) (va_arg(args, int) && 0x00FF);
          *p++ = (char)b;
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
