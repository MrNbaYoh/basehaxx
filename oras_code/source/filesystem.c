#include "filesystem.h"

Result _FSUSER_OpenFile(Handle* handle, Handle* out, FS_Archive archive, FS_Path fileLowPath, u32 openflags, u32 attributes)
{
    u32* cmdbuf=getThreadCommandBuffer();
    cmdbuf[0]=0x080201C2;
    cmdbuf[1]=0;
    cmdbuf[2]=(u32) archive.handle;
    cmdbuf[3]=(u32) (archive.handle >> 32);
    cmdbuf[4]=fileLowPath.type;
    cmdbuf[5]=fileLowPath.size;
    cmdbuf[6]=openflags;
    cmdbuf[7]=attributes;
    cmdbuf[8]=(fileLowPath.size<<14)|2;
    cmdbuf[9]=(u32)fileLowPath.data;
    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;
    if(out)*out=cmdbuf[3];
    return cmdbuf[1];
}

Result _FSUSER_OpenArchive(Handle *handle, FS_Archive *archive)
{
    if(!archive)
        return -2;

    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = 0x080C00C2;
    cmdbuf[1] = archive->id;
    cmdbuf[2] = archive->lowPath.type;
    cmdbuf[3] = archive->lowPath.size;
    cmdbuf[4] = (archive->lowPath.size << 14) | 0x2;
    cmdbuf[5] = (u32)archive->lowPath.data;

    Result ret = 0;
    if((ret = svcSendSyncRequest(*handle)))
        return ret;

    archive->handle  = cmdbuf[2] | ((u64)cmdbuf[3] << 32);

    return cmdbuf[1];
}

Result _FSFILE_Close(Handle handle)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x08080000;

    Result ret=0;
    if((ret=svcSendSyncRequest(handle)))return ret;

    return cmdbuf[1];
}

Result _FSFILE_Read(Handle handle, u32 *bytesRead, u64 offset, u32 *buffer, u32 size)
{
    u32 *cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x080200C2;
    cmdbuf[1]=(u32)offset;
    cmdbuf[2]=(u32)(offset>>32);
    cmdbuf[3]=size;
    cmdbuf[4]=(size<<4)|12;
    cmdbuf[5]=(u32)buffer;

    Result ret=0;
    if((ret=svcSendSyncRequest(handle)))return ret;

    if(bytesRead)*bytesRead=cmdbuf[2];

    return cmdbuf[1];
}

Result _FSFILE_GetSize(Handle handle, u64 *size)
{
    u32 *cmdbuf=getThreadCommandBuffer();

    cmdbuf[0] = 0x08040000;

    Result ret=0;
    if((ret=svcSendSyncRequest(handle)))return ret;

    if(size)*size = *((u64*)&cmdbuf[2]);

    return cmdbuf[1];
}

Result _FSFILE_Write(Handle handle, u32 *bytesWritten, u64 offset, u32 *data, u32 size, u32 flushFlags)
{
    u32 *cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x08030102;
    cmdbuf[1]=(u32)offset;
    cmdbuf[2]=(u32)(offset>>32);
    cmdbuf[3]=size;
    cmdbuf[4]=flushFlags;
    cmdbuf[5]=(size<<4)|10;
    cmdbuf[6]=(u32)data;

    Result ret=0;
    if((ret=svcSendSyncRequest(handle)))return ret;

    if(bytesWritten)*bytesWritten=cmdbuf[2];

    return cmdbuf[1];
}

Result _FSUSER_DeleteFile(Handle *handle, FS_Archive archive, FS_Path fileLowPath)
{
    u32 *cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x08040142;
    cmdbuf[1]= 0;
    cmdbuf[2]=(u32)archive.handle;
    cmdbuf[3]=(u32)(archive.handle >> 32);
    cmdbuf[4]=fileLowPath.type;
    cmdbuf[5]=fileLowPath.size;
    cmdbuf[6]=(fileLowPath.size << 14) | 0x2;
    cmdbuf[7]=(u32)fileLowPath.data;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _FSUSER_ControlArchive(Handle *handle, FS_Archive archive, FS_ArchiveAction action, void* input, u32 inputSize, void* output, u32 outputSize)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x080d0144;
    cmdbuf[1]=(u32) archive.handle;
    cmdbuf[2]=(u32) (archive.handle >> 32);
    cmdbuf[3]=action;
    cmdbuf[4]=inputSize;
    cmdbuf[5]=outputSize;
    cmdbuf[6]=(inputSize << 4) | 0x8 | 0x2;
    cmdbuf[7]=(u32)input;
    cmdbuf[8]=(outputSize << 4) | 0x8 | 0x4;
    cmdbuf[9]=(u32)output;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _FSUSER_CloseArchive(Handle* handle, FS_Archive* archive)
{
    if(!archive)return -2;
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x080E0080;
    cmdbuf[1]=(u32) archive->handle;
    cmdbuf[2]=(u32) (archive->handle >> 32);

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}