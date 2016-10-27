#ifndef UTILS_H
#define UTILS_H

#include <3ds.h>

void* memset(void * ptr, int value, size_t num);
void* memcpy(void *destination, const void *source, size_t num);

unsigned short ccitt16(unsigned char *data, unsigned int length);

Result gspwn(void* dst, void* src, u32 size);
Result _GSPGPU_SetBufferSwap(Handle handle, u32 screenId, GSPGPU_FramebufferInfo framebufferInfo);
Result _GSPGPU_InvalidateDataCache(Handle handle, const void* adr, u32 size);

unsigned int _strlen(const char* str);

FS_Path _fsMakePath(FS_PathType type, const void* path);
#endif