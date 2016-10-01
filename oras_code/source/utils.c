#include "utils.h"
#include "imports.h"

int memcmp(void *ptr1, void *ptr2, size_t num)
{
	for(; num--; ptr1++, ptr2++)
		if(*(u8*)ptr1 != *(u8*)ptr2)
			return *(u8*)ptr1-*(u8*)ptr2;
			
	return 0;
}

void* memset(void *ptr, int value, size_t num)
{
    u8 *p = ptr;
    while (num) {
        *p++ = value;
        num--;
    }
	
	return ptr;
}

void* memcpy(void *destination, const void *source, size_t num)
{
    u8 *dest = destination;
    u8 *src  = (u8*)source;
    while (num) {
        *dest++ = *src++;
        num--;
    }
	
	return destination;
}

size_t total_size = 0;

void* calloc(size_t num, size_t size)
{
	u8* ptr = &LINEAR_BUFFER[0x00200000 + total_size];
	total_size += num*size;
	memset(ptr, 0, num*size);
	return ptr;
}

Result gspwn(void* dst, void* src, u32 size)
{
	u32 gxCommand[] = 
	{ 			
		0x00000004, //cmd header (SetTextureCopy)
		(u32)src, 
		(u32)dst,
		size,
		0xFFFFFFFF, //dim in
		0xFFFFFFFF, //dim out
		0x00000008, //flags
		0x00000000
	};
	
	return _GSPGPU_GxTryEnqueue(sharedGspCmdBuf, gxCommand);
}

Result _GSPGPU_SetBufferSwap(Handle handle, u32 screenId, GSPGPU_FramebufferInfo framebufferInfo)
{
	Result ret = 0;
	u32* cmdbuf = getThreadCommandBuffer();
	
	cmdbuf[0] = 0x0050200;
	cmdbuf[1] = screenId;
	memcpy(&cmdbuf[2], &framebufferInfo, sizeof(GSPGPU_FramebufferInfo));
	
	if((ret = svcSendSyncRequest(handle))) return ret;
	
	return cmdbuf[1];
}


Result _GSPGPU_InvalidateDataCache(Handle handle, const void* adr, u32 size)
{
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x90082;
	cmdbuf[1] = (u32)adr;
	cmdbuf[2] = size;
	cmdbuf[3] = 0;
	cmdbuf[4] = 0xFFFF8001;

	Result ret=0;
	if(R_FAILED(ret=svcSendSyncRequest(handle)))return ret;

	return cmdbuf[1];
}


unsigned int _strlen(const char* str)
{
	unsigned int length = 0;
	while(*(str++)) length++;
	return length;
}

Result _srvGetServiceHandle(Handle *out, const char* srvName)
{
	return _SRV_GetServiceHandle(out, srvName, _strlen(srvName));
}

FS_Path _fsMakePath(FS_PathType type, const void* path)
{
	FS_Path p = {type, 0, path};
    switch(type)
    {
        case PATH_ASCII:
            p.size = _strlen((const char*)path) + 1;
            break;
        case PATH_UTF16:
        {
            const u16* str = (const u16*)path;
            while (*str++) p.size++;
            p.size = (p.size + 1 ) * 2;
            break;
        }
        case PATH_EMPTY:
            p.size = 1;
            p.data = "";
        default:
            break;
    }
	
    return p;
}