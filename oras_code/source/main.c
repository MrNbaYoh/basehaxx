#include <string.h>

#include "imports.h"

#include <3ds.h>
#include "utils.h"
#include "screen_draw.h"
#include "hid.h"
#include "xprintf.h"
#include "filesystem.h"

Result loadOtherAppPayload(u32* out_size)
{
	Result ret = 0;

	FS_Archive archive = 0;
	ret = _FSUSER_OpenArchive(fsHandle, &archive, ARCHIVE_SDMC, _fsMakePath(PATH_EMPTY, ""));
	if(ret) return ret;
	
	Handle file = 0;
	ret = _FSUSER_OpenFile(fsHandle, &file, archive, _fsMakePath(PATH_ASCII, "/basehaxx/otherapp.bin"), FS_OPEN_READ, 0);
	if(ret) return ret;
	
	u64 size = 0;
	ret = _FSFILE_GetSize(file, &size);
	if(ret) return ret;
	
	*out_size = (u32)size; 
	
	u32 bytes_read = 0;
	ret = _FSFILE_Read(file, &bytes_read, 0, (u32*)LINEAR_BUFFER, *out_size);
	if(ret) return ret;
	
	ret = _FSFILE_Close(file);
	if(ret) return ret;
	
	ret = _FSUSER_CloseArchive(fsHandle, archive);
	return ret;
}

Result uninstallExploit()
{
	Result ret = 0;
	u8 hax_string[] = {'H', 0, 'a', 0, 'x', 0, 'x', 0};
	
	FS_Archive archive = 0;
	ret = _FSUSER_OpenArchive(fsHandle, &archive, ARCHIVE_SAVEDATA, _fsMakePath(PATH_EMPTY, ""));
	if(ret) return ret;
		
	Handle file = 0;
	ret = _FSUSER_OpenFile(fsHandle, &file, archive, _fsMakePath(PATH_ASCII, "/main"), FS_OPEN_READ | FS_OPEN_WRITE, 0);
	if(ret) return ret;
	
	u8 array_uninstall[ORAS_BASE_MSG_LENTGH];
	memset(array_uninstall, 0, ORAS_BASE_MSG_LENTGH);
	memcpy(array_uninstall, hax_string, 0x8);
	memcpy(array_uninstall+0x1A, hax_string, 0x8);
	
	u8 base_fav[ORAS_BASE_FAV_LENTGH];
	memset(base_fav, 0, ORAS_BASE_FAV_LENTGH);
	
	u32 bytes_read = 0;
	ret = _FSFILE_Read(file, &bytes_read, ORAS_BASE_FAV_OFFSET, (u32*)base_fav, ORAS_BASE_FAV_LENTGH);
	if(ret) return ret;
	
	memcpy(base_fav+(ORAS_SAVE_BASE_OWNERNAME_OFFSET-ORAS_BASE_FAV_OFFSET), array_uninstall, ORAS_BASE_MSG_LENTGH);
	
	u32 bytes_written = 0;
	ret = _FSFILE_Write(file, &bytes_written, ORAS_BASE_FAV_OFFSET, (u32*)base_fav, ORAS_BASE_FAV_LENTGH, FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
	if(ret) return ret;
	
	u16 chk = ccitt16(base_fav, ORAS_BASE_FAV_LENTGH);
	ret = _FSFILE_Write(file, &bytes_written, ORAS_BASE_FAV_CHK_OFFSET, (u32*)&chk, sizeof(u16), FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
	if(ret) return ret;
	
	ret = _FSFILE_Close(file);
    if(ret) return ret;
	
	ret = _FSUSER_ControlArchive(fsHandle, archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
    if(ret) return ret;

    ret = _FSUSER_CloseArchive(fsHandle, archive);
	return ret;
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

Result displayMenu(u8* fb, u32* size_out)
{	
	drawString(fb, "Launch *hax payload", 40, 64);
    drawString(fb, "Uninstall Basehaxx", 40, 72);
	drawString(fb, "Exit & Reboot", 40, 80);
	
	u8 cursorPos = 0;
	u32 key;
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
			switch(cursorPos)
			{
				case 0:
				{
					return loadOtherAppPayload(size_out);
				}
				
				case 1:
				{
					uninstallExploit();
					break;
				}
				
				default:
				{
					svcExitProcess();
					break;
				}
			}
				
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
	
	_DSP_UnloadComponent(dspHandle);
	_DSP_RegisterInterruptEvents(dspHandle, 0x0, 0x2, 0x2);
	
	hidKeys = (u32*)((*(u32*)ORAS_HID_SHAREDMEM_PTR) + 0x1C);
	Result ret = 0;
	
	u8* top_framebuffer = &LINEAR_BUFFER[0x00100000];
    u8* low_framebuffer = &top_framebuffer[0x00046500];
    _GSPGPU_SetBufferSwap(*gspHandle, 0, (GSPGPU_FramebufferInfo){0, (u32*)top_framebuffer, (u32*)top_framebuffer, 240 * 3, (1<<8)|(1<<6)|1, 0, 0});
    _GSPGPU_SetBufferSwap(*gspHandle, 1, (GSPGPU_FramebufferInfo){0, (u32*)low_framebuffer, (u32*)low_framebuffer, 240 * 3, 1, 0, 0});
	
	u32 otherapp_size = 0;
	
	if(hidKey() & BUTTON_START)
	{
		clearScreenColor(top_framebuffer, TOTAL_SCREEN_SIZE, 50<<16 | 50<<8 | 50);
		_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, (u32*)&LINEAR_BUFFER[0x00100000], (400*240*3)+(320*240*3));
	
		centerString(top_framebuffer, "basehaxx in-game menu", 16, 400);
	
		displayMenu(top_framebuffer, &otherapp_size);
	}
	else
		loadOtherAppPayload(&otherapp_size);
		
	otherapp_size = (otherapp_size + 0xFFF) & ~0xFFF;
		
	_GSPGPU_FlushDataCache(gspHandle, 0xFFFF8001, (u32*)LINEAR_BUFFER, otherapp_size);
	for(int i = 0; i < 0x7; i++)
	{
		gspwn((void*)(linear_base + otherapp_pages[i]), (void*)(LINEAR_BUFFER+i*0x1000), 0x1000);
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