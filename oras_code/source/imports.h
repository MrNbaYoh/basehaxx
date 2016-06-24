#ifndef IMPORTS_H
#define IMPORTS_H

#include <3ds.h>
#include "../../build/constants.h"

#define LINEAR_BUFFER ((u8*)0x14100000)
#define PICDATA_SAVE_OFFSET 0x67C00
#define PICDATA_SIZE 0xE058
#define PICDATA_CHK_OFFSET 0x75FE2
#define HAX_PAYLOAD_OFFSET 0x5000
#define HAX_PAYLOAD_ADDR (ORAS_SAVE_PICDATA_BUFFER_PTR + HAX_PAYLOAD_OFFSET)

extern u32* hidKeys;
extern Handle httpcHandle;

static Handle* const fsHandle = (Handle*)ORAS_FSUSER_HANDLE;
static Handle* const dspHandle = (Handle*)ORAS_DSP_HANDLE;
static Handle* const gspHandle = (Handle*)ORAS_GSPGPU_HANDLE;

static u32** const sharedGspCmdBuf = (u32**)(ORAS_GSPGPU_INTERRUPT_RECEIVER_STRUCT + 0x58);

static Result (* const _GSPGPU_FlushDataCache)(Handle* handle, Handle kProcess, u32* addr, u32 size) = (void*)ORAS_GSPGPU_FLUSHDATACACHE;
static Result (* const _GSPGPU_GxTryEnqueue)(u32** sharedGspCmdBuf, u32* cmdAddr) = (void*)ORAS_GSPGPU_GXTRYENQUEUE;
static Result (* const _DSP_UnloadComponent)(Handle* handle) = (void*)ORAS_DSP_UNLOADCOMPONENT;
static Result (* const _DSP_RegisterInterruptEvents)(Handle* handle, Handle event, u32 type, u32 port) = (void*)ORAS_DSP_REGISTERINTERRUPTEVENTS; 

static Result (* const _SRV_GetServiceHandle)(Handle *out, const char* srvName, u8 nameLength) = (void*)ORAS_SRV_GETSERVICEHANDLE;

static const char regions[7][4] = 
{
	"JPN",
	"USA",
	"EUR",
	"EUR",
	"CHN",
	"KOR",
	"TWN"
};

extern u32 screenColor;
#endif