#ifndef ARDPRINTF
#define ARDPRINTF
#define ARDBUFFER 16
#include "core/Arduino.h"


// int l=2;
// char *j = "test";
// long k = 123456789;
// char s = 'g';
// float f = 2.3;
//
// ardprintf("test %d %l %c %s %f", l, k, s, j, f);

class S {
public:
    static int print(String str, ...) {
      int i, count=0, j=0;
      char temp[ARDBUFFER+1];
      for(i=0; str[i]!='\0';i++)  if(str[i]=='%')  count++;

      va_list argv;
      va_start(argv, count);
      for(i=0,j=0; str[i]!='\0';i++)
      {
        if(str[i]=='%')
        {
          temp[j] = '\0';
          Serial.print(temp);
          j=0;
          temp[0] = '\0';

          switch(str[++i])
          {
            case 'd': Serial.print(va_arg(argv, int));
                      break;
            case 'l': Serial.print(va_arg(argv, long));
                      break;
            case 'f': Serial.print(va_arg(argv, double));
                      break;
            case 'c': Serial.print((char)va_arg(argv, int));
                      break;
            case 's': Serial.print(va_arg(argv, char *));
                      break;
            default:  ;
          };
        }
        else
        {
          temp[j] = str[i];
          j = (j+1)%ARDBUFFER;
          if(j==0)
          {
            temp[ARDBUFFER] = '\0';
            Serial.print(temp);
            temp[0]='\0';
          }
        }
      };
      Serial.println();
      return count + 1;
    }
};

#undef ARDBUFFER
#endif
