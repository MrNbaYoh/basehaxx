#ifndef HID_H
#define HID_H

#include <3ds.h>

#define BUTTON_A      (1 << 0)
#define BUTTON_B      (1 << 1)
#define BUTTON_SELECT (1 << 2)
#define BUTTON_START  (1 << 3)
#define BUTTON_RIGHT  (1 << 4)
#define BUTTON_LEFT   (1 << 5)
#define BUTTON_UP     (1 << 6)
#define BUTTON_DOWN   (1 << 7)
#define BUTTON_R      (1 << 8)
#define BUTTON_L      (1 << 9)
#define BUTTON_X      (1 << 10)
#define BUTTON_Y      (1 << 11)

u32 hidKey();
u32 hidWaitKey();

#endif