#ifndef DEFAULT_PIN
#define DEFAULT_PIN

#define TRUE_PIN -1
#define FALSE_PIN 0

#define MINIMUM_DEFAULT_VALUE -1
#endif


#ifndef IN_PIN
#define IN_PIN 16
#define IN_PIN_START MINIMUM_DEFAULT_VALUE
#endif 

#ifndef OUT_PIN
#define OUT_PIN 16
#define OUT_PIN_START IN_PIN_START-IN_PIN
#endif

#ifndef USER_PIN
#define USER_PIN
#define USER_PIN_START OUT_PIN_START-OUT_PIN
#endif


#ifndef SB
#define SB 3
#define SB_LUT4 1
#define SB_DFF 2

#endif
