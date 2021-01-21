/*---- start macro's ------------------------------------------------------------------*/

#define SPrintFlush() ({ if (__AVR_ATmega328P__)               \
                          { Serial.flush(); \
                          } \
                      })

#define SPrint(...)   ({ if (__AVR_ATmega328P__)               \
                          { Serial.print(__VA_ARGS__);         \
                          }  \
                       })
#define SPrintln(...)  ({ if (__AVR_ATmega328P__)               \
                           { Serial.println(__VA_ARGS__);       \
                           }  \
                        })
