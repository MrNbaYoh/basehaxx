#ifndef SCREEN_DRAW_H
#define SCREEN_DRAW_H

#include <3ds.h>

#define TOTAL_SCREEN_SIZE (400*240*3)+(320*240*3)
#define TOP_SCREEN_SIZE (400*240*3)
#define BOTTOM_SCREEN_SIZE (320*240*3)

void clearScreenColor(u8* fb, u32 size, u32 color);
void clearScreen(u8* fb, u32 size);
void eraseChar(u8* fb, u16 x, u16 y);
void drawCharacter(u8* fb, char c, u16 x, u16 y, u32 color);
void drawStringColor(u8* fb, char* str, u16 x, u16 y, u32 color);
void drawString(u8* fb, char* str, u16 x, u16 y);
void centerString(u8* fb, char* str, u16 y, u16 screen_x);

#endif