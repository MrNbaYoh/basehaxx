#include "httpc.h"
#include "utils.h"

Result _HTTPC_Initialize(Handle* handle)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x10044; 
    cmdbuf[1]=0x1000;
    cmdbuf[2]=0x20;
	cmdbuf[3]=0x0;
	cmdbuf[4]=0x0;
	cmdbuf[5]=0xFFFFFFFF;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _HTTPC_CreateContext(Handle* handle, char* url, Handle* contextHandle)
{
    u32* cmdbuf=getThreadCommandBuffer();
    u32 l=_strlen(url)+1;

    cmdbuf[0]=0x20082; 
    cmdbuf[1]=l;
    cmdbuf[2]=0x01; 
    cmdbuf[3]=(l<<4)|0xA;
    cmdbuf[4]=(u32)url;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    if(contextHandle)*contextHandle=cmdbuf[2];

    return cmdbuf[1];
}

Result _HTTPC_AddRequestHeaderField(Handle* handle, Handle contextHandle, char* name, char* value)
{
    u32* cmdbuf=getThreadCommandBuffer();

    int name_len=_strlen(name)+1;
    int value_len=_strlen(value)+1;

    cmdbuf[0]=0x1100C4;
    cmdbuf[1]=contextHandle;
    cmdbuf[2]=name_len;
    cmdbuf[3]=value_len;
    cmdbuf[4]=IPC_Desc_StaticBuffer(name_len,3);
    cmdbuf[5]=(u32)name;
    cmdbuf[6]=IPC_Desc_Buffer(value_len,IPC_BUFFER_R);
    cmdbuf[7]=(u32)value;

    Result ret=0;
    if(R_FAILED(ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _HTTPC_GetResponseStatusCode(Handle* handle, Handle contextHandle, u32* out, u64 delay)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x220040;
    cmdbuf[1]=contextHandle;

    Result ret=0;
    if(R_FAILED(ret=svcSendSyncRequest(*handle)))return ret;

    *out = cmdbuf[2];

    return cmdbuf[1];
}

Result _HTTPC_GetResponseHeader(Handle* handle, Handle contextHandle, char* name, char* value, u32 valuebuf_maxsize)
{
    u32* cmdbuf=getThreadCommandBuffer();

    int name_len=_strlen(name)+1;

    cmdbuf[0]=0x1E00C4;
    cmdbuf[1]=contextHandle;
    cmdbuf[2]=name_len;
    cmdbuf[3]=valuebuf_maxsize;
    cmdbuf[4]=IPC_Desc_StaticBuffer(name_len, 3);
    cmdbuf[5]=(u32)name;
    cmdbuf[6]=IPC_Desc_Buffer(valuebuf_maxsize, IPC_BUFFER_W);
    cmdbuf[7]=(u32)value;

    Result ret=0;
    if(R_FAILED(ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _HTTPC_InitializeConnectionSession(Handle* handle, Handle contextHandle)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x80042; 
    cmdbuf[1]=contextHandle;
    cmdbuf[2]=0x20; 

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _HTTPC_SetProxyDefault(Handle* handle, Handle contextHandle)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0xe0040;
    cmdbuf[1]=contextHandle;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _HTTPC_CloseContext(Handle* handle, Handle contextHandle)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x30040; 
    cmdbuf[1]=contextHandle;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _HTTPC_BeginRequest(Handle* handle, Handle contextHandle)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x90040; 
    cmdbuf[1]=contextHandle;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _HTTPC_ReceiveData(Handle* handle, Handle contextHandle, u8* buffer, u32 size)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0xB0082; 
    cmdbuf[1]=contextHandle;
    cmdbuf[2]=size;
    cmdbuf[3]=(size<<4)|12;
    cmdbuf[4]=(u32)buffer;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    return cmdbuf[1];
}

Result _HTTPC_GetDownloadSizeState(Handle* handle, Handle contextHandle, u32* totalSize)
{
    u32* cmdbuf=getThreadCommandBuffer();

    cmdbuf[0]=0x00060040;
    cmdbuf[1]=contextHandle;

    Result ret=0;
    if((ret=svcSendSyncRequest(*handle)))return ret;

    if(totalSize)*totalSize=cmdbuf[3];

    return cmdbuf[1];
}