
/*---- D-macro's for Debugging - don't use in production! ------*/
  #define DFlush()     ({ if (_DEBUG)     \
                            { Serial.flush(); \
                            } \
                        })

  #define DPrint(...)   ({ if (_DEBUG)     \
                            { Serial.print(__VA_ARGS__);      \
                            }  \
                         })
  #define DPrintln(...)  ({ if (_DEBUG)     \
                             { Serial.println(__VA_ARGS__);   \
                             }  \
                          })

/*---- S-macro's send data to the ESP8266 ----------------------*/
  #define SFlush()        ({ Serial.flush(); \
                          })
  #define SPrint(...)     ({ Serial.print(__VA_ARGS__);        \
                          })
  #define SPrintln(...)   ({  Serial.println(__VA_ARGS__);     \
                          })
