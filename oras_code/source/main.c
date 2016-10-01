#include <string.h>

#include "imports.h"

#include <3ds.h>
#include "utils.h"
#include "screen_draw.h"
#include "hid.h"
#include "updater.h"
#include "httpc.h"
#include "decomp.h"
#include "xprintf.h"
#include "filesystem.h"

void firmware_select(u8* framebuf, u8* firmware_version)
{
	u8* fb = &framebuf[0x46500];
	int firmware_selected_value = 0;
	while(1)
	{
		u32 input = hidWaitKey();

		if(input & KEY_LEFT) firmware_selected_value--;
		if(input & KEY_RIGHT) firmware_selected_value++;

		if(firmware_selected_value < 0) firmware_selected_value = 0;
		else if(firmware_selected_value > 5) firmware_selected_value = 5;

		int firmware_maxnum = 256;
		if(firmware_selected_value == 0) firmware_maxnum = 2;
		else if(firmware_selected_value == 5) firmware_maxnum = 7;
		
		if(input & KEY_UP && firmware_version[firmware_selected_value] < firmware_maxnum-1) firmware_version[firmware_selected_value]++;
		if(input & KEY_DOWN && firmware_version[firmware_selected_value] > 0) firmware_version[firmware_selected_value]--;

		if(input & KEY_A) break;

		char *str_top = (firmware_version[firmware_selected_value] < firmware_maxnum - 1) ? "^" : "-";
		char *str_bottom = (firmware_version[firmware_selected_value] > 0) ? "v" : "-";

		int offset = 19;
		if(firmware_selected_value)
		{
			offset += 7;

			for(int i = 1; i < firmware_selected_value; i++)
			{
				offset += 2;
				if(firmware_version[i] >= 10) offset++;
			}

			if(firmware_selected_value >= 5)
			{
				offset += 1;
			}
		}

		clearScreen(fb, BOTTOM_SCREEN_SIZE);
		
		char *str = (char*)&LINEAR_BUFFER[0x0017E900];

		xsprintf(str, "Selected firmware: %s %d-%d-%d-%d %s  \n", firmware_version[0] ? "New3DS" : "Old3DS", firmware_version[1], firmware_version[2], firmware_version[3], firmware_version[4], regions[firmware_version[5]]);
		drawString(fb, str, 0, 16);

		drawString(fb, str_top, offset * 8, 8);
		drawString(fb, str_bottom, offset * 8, 24);
	}
}

void decompressPayload(u8** decompressed_buffer, u32* decompressed_size)
{
	u32 compressed_size = *(u32*)(HAX_PAYLOAD_ADDR - 0x4);
	memcpy(LINEAR_BUFFER, (void*)HAX_PAYLOAD_ADDR, compressed_size);
	
	*decompressed_buffer = &LINEAR_BUFFER[(compressed_size + 0xfff) & ~0xfff];
	*decompressed_size = lzss_get_decompressed_size(LINEAR_BUFFER, compressed_size);

	lzss_decompress(LINEAR_BUFFER, compressed_size, *decompressed_buffer, *decompressed_size);
}

u32 mod3(u32 n) 
{
   u32 r;
   r = (0x55555555*n + (n >> 1) - (n >> 3)) >> 30;
   return r;//(n - r)*0xAAAAAAAB;
}

void drawCursor(u8* fb, u8 pos)
{
	drawStringColor(fb, ">", 24, 64 + 8 * pos, 0xFFFFFF);
	eraseChar(fb, 24, 64 + 8 * mod3(pos + 1));
	eraseChar(fb, 24, 64 + 8 * mod3(pos + 2));
}

void displayMenu(u8* fb, u8** decompressed_buffer, u32* decompressed_size)
{
	
	*decompressed_buffer = LINEAR_BUFFER;
	
	drawString(fb, "Launch *hax payload", 40, 64);
    drawString(fb, "Update *hax payload", 40, 72);
	drawString(fb, "Exit & Reboot", 40, 80);
	
	u8 cursorPos = 0;
	u32 key;
	bool updated = false;
	while(true)
	{
		drawCursor(fb, cursorPos);
		
		key = hidWaitKey();
		if(key & BUTTON_UP)
			cursorPos = mod3(cursorPos + 3 - 1);
		if(key & BUTTON_DOWN)
			cursorPos = mod3(cursorPos + 1);
		if(key & BUTTON_A)
		{
			if(cursorPos == 0)
			{
				if(!updated) decompressPayload(decompressed_buffer, decompressed_size);
				break;
			}
			else if(cursorPos == 1)
			{
				Result ret = 0;
				if(httpcHandle == 0)
				{
					ret = _srvGetServiceHandle(&httpcHandle, "http:C");
					if(!ret) ret = _HTTPC_Initialize(&httpcHandle);
					if(ret)
					{
						drawString(&fb[0x00046500], "Failed to initialize httpc.", 0, 16);
					}
				}
				
				if(httpcHandle != 0)
				{
					u8 firmware_version[6] = {0};
					firmware_version[0] = 0;
					firmware_version[5] = 3;
					firmware_version[1] = 11;
					firmware_version[2] = 0;
					firmware_version[3] = 0;
					firmware_version[4] = 33;
					
					firmware_select(fb, firmware_version);
					ret = update(firmware_version, decompressed_size);
					clearScreen(&fb[0x00046500], BOTTOM_SCREEN_SIZE);
					if(!ret)
					{
						updated = true;
						drawString(&fb[0x00046500], "*hax payload successfully updated", 0, 16);
					}
					else drawString(&fb[0x00046500], "Failed to update.", 0, 16);
				}
			}
			else
				svcExitProcess();
		}
	}
}

u32* hidKeys;
Handle httpcHandle;

void _main()
{
	u32 otherapp_pages[0x7];
	memset(otherapp_pages, 0x0, 0x4*0x7);
	
	u32 linear_base = 0x14000000 + (*(u8*)ORAS_APPMEMTYPE_PTR == 0x6 ? 0x07c00000 : 0x04000000) - ORAS_MAX_CODEBIN_SIZE;
	
	for(unsigned int i = 0, l = 0x14000000, pages = 0; i < ORAS_MAX_CODEBIN_SIZE && pages < 0x7; i+=0x1000, l+=0x20)
	{
		gspwn((void*)l, (void*)(linear_base + i), 0x1000);
		svcSleepThread(0x100000);
		
		for(u8 j = 0; j<0x7; j++)
		{
			if(!memcmp((void*)l, (void*)(0x101000 + j*0x1000), 0x20))
			{
				otherapp_pages[j] = i;
				pages++;
			}
		}
	}

	
	hidKeys = (u32*)((*(u32*)ORAS_HID_SHAREDMEM_PTR) + 0x1C);
	httpcHandle = 0;
	Result ret = 0;
	
	u8* top_framebuffer = &LINEAR_BUFFER[0x00100000];
    u8* low_framebuffer = &top_framebuffer[0x00046500];
    _GSPGPU_SetBufferSwap(*gspHandle, 0, (GSPGPU_FramebufferInfo){0, (u32*)top_framebuffer, (u32*)top_framebuffer, 240 * 3, (1<<8)|(1<<6)|1, 0, 0});
    _GSPGPU_SetBufferSwap(*gspHandle, 1, (GSPGPU_FramebufferInfo){0, (u32*)low_framebuffer, (u32*)low_framebuffer, 240 * 3, 1, 0, 0});

	clearScreenColor(top_framebuffer, TOTAL_SCREEN_SIZE, 50<<16 | 50<<8 | 50);
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, (u32*)&LINEAR_BUFFER[0x00100000], (400*240*3)+(320*240*3));
	
	centerString(top_framebuffer, "basehaxx in-game menu", 16, 400);
	
	u8* decompressed_buffer = NULL;
	u32 decompressed_size = 0;
	
	displayMenu(top_framebuffer, &decompressed_buffer, &decompressed_size);
	
	_DSP_UnloadComponent(dspHandle);
	_DSP_RegisterInterruptEvents(dspHandle, 0x0, 0x2, 0x2);
	
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, (u32*)decompressed_buffer, decompressed_size);
	//gspwn((void*)(code_linear_base + 0x00101000 - 0x00100000), decompressed_buffer, (decompressed_size + 0x1F) & ~0x1F);
	for(int i = 0; i < 0x7; i++)
	{
		gspwn((void*)(linear_base + otherapp_pages[i]), (void*)(decompressed_buffer+i*0x1000), 0x1000);
		svcSleepThread(0x100000);
	}
	
	// ghetto dcache invalidation
	// don't judge me
	int i, j;
	// for(k=0; k<0x2; k++)
		for(j=0; j<0x4; j++)
			for(i=0; i<0x01000000/0x4; i+=0x4)
				LINEAR_BUFFER[i+j]^=0xDEADBABE;
	
	// run payload
	{
		void (*payload)(u32* paramlk, u32* stack_pointer) = (void*)0x00101000;
		u32* paramblk = (u32*)LINEAR_BUFFER;

		paramblk[0x1c >> 2] = ORAS_GSPGPU_GXCMD4;
		paramblk[0x20 >> 2] = ORAS_GSPGPU_FLUSHDATACACHE_WRAPPER;
		paramblk[0x48 >> 2] = 0x8d; // flags
		paramblk[0x58 >> 2] = ORAS_GSPGPU_HANDLE;
		paramblk[0x64 >> 2] = 0x08010000;

		payload(paramblk, (u32*)(0x10000000 - 4));
	}

	*(u32*)ret = 0xdead0008;
}