#ifndef wang_led_hongge
#define wang_led_hongge

#include "sys.h"
#define led0 PCout(6)
#define led1 PCout(7)
#define led2 PCout(8)
#define key0 PDin(12)


void led_init(void);

#endif
