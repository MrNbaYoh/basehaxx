#ifndef IMPORTS_H
#define IMPORTS_H

#include <3ds.h>
#include "../../build/constants.h"

#define LINEAR_BUFFER ((u8*)0x14100000)
#define ORAS_MAX_CODEBIN_SIZE 0x5AF000
#define ORAS_APPMEMTYPE_PTR 0x1FF80030
#define ORAS_SAVE_BASE_OWNERNAME_OFFSET 0x23F3E
#define ORAS_BASE_MSG_LENTGH 0xE6
#define ORAS_BASE_FAV_OFFSET 0x23A00
#define ORAS_BASE_FAV_LENTGH 0x7AD0
#define ORAS_BASE_FAV_CHK_OFFSET 0x75FCA

extern u32* hidKeys;

static Handle* const fsHandle = (Handle*)ORAS_FSUSER_HANDLE;
static Handle* const dspHandle = (Handle*)ORAS_DSP_HANDLE;
static Handle* const gspHandle = (Handle*)ORAS_GSPGPU_HANDLE;

static u32** const sharedGspCmdBuf = (u32**)(ORAS_GSPGPU_INTERRUPT_RECEIVER_STRUCT + 0x58);

static Result (* const _GSPGPU_FlushDataCache)(Handle* handle, Handle kProcess, u32* addr, u32 size) = (void*)ORAS_GSPGPU_FLUSHDATACACHE;
static Result (* const _GSPGPU_GxTryEnqueue)(u32** sharedGspCmdBuf, u32* cmdAddr) = (void*)ORAS_GSPGPU_GXTRYENQUEUE;
static Result (* const _DSP_UnloadComponent)(Handle* handle) = (void*)ORAS_DSP_UNLOADCOMPONENT;
static Result (* const _DSP_RegisterInterruptEvents)(Handle* handle, Handle event, u32 type, u32 port) = (void*)ORAS_DSP_REGISTERINTERRUPTEVENTS; 

extern u32 screenColor;
#endif