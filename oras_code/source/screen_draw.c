#include "screen_draw.h"
#include "font.h"
#include "utils.h"

u32 screenColor = 0x0;

void clearScreenColor(u8* fb, u32 size, u32 color)
{
	u32 num = size;
	u8 *p = fb;
    while (num) {
        *p++ = color&0xFF;
		*p++ = (color&0xFF00)>>8;
		*p++ = (color&0xFF0000)>>16;
        num-=3;
    }
	
	screenColor = color;
}

void clearScreen(u8* fb, u32 size)
{
	clearScreenColor(fb, size, screenColor);
}

void eraseChar(u8* fb, u16 x, u16 y)
{
	y=232-y;
    int i, j;
    for (i = 7; i >= 0; --i)
    {
        for (j = 0; j < 8; ++j)
        {
            u8 *pixel = (u8*)(fb+(((y+i) - 1) * 3 + (x+j) * 3 * 240));
            pixel[0]=screenColor&0xFF;
            pixel[1]=(screenColor&0xFF00)>>8;
            pixel[2]=(screenColor&0xFF0000)>>16;
        }
    }
}

void drawCharacter(u8* fb, char c, u16 x, u16 y, u32 color)
{
    u8 *font = (u8*)(msx_font + c * 8);
    int i, j;
    for (i = 7; i >= 0; --i)
    {
        for (j = 0; j < 8; ++j)
        {
            u8 *pixel = (u8*)(fb+(((y+i) - 1) * 3 + (x+j) * 3 * 240));

            //pixel[0]=pixel[1]=pixel[2]=0x0;
            if ((*font & (128 >> j)))
            {
                pixel[0]=color&0xFF;
                pixel[1]=(color&0xFF00)>>8;
                pixel[2]=(color&0xFF0000)>>16;
            }
        }
        ++font;
    }
}

void drawStringColor(u8* fb, char* str, u16 x, u16 y, u32 color)
{
    if(!str)return;
    y=232-y;
    int k;
    int dx=0, dy=0;
    for(k=0;k<_strlen(str);k++)
    {
        if(str[k]>=32 && str[k]<128)drawCharacter(fb,str[k],x+dx,y+dy,color);
        dx+=8;
        if(str[k]=='\n'){dx=0;dy-=8;}
    }
}

void drawString(u8* fb, char* str, u16 x, u16 y)
{
    drawStringColor(fb,str,x,y,0xFFFFFF);
}

void centerString(u8* fb, char* str, u16 y, u16 screen_x)
{
    drawString(fb, str, (screen_x-(_strlen(str)*8))/2, y);
}