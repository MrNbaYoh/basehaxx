#ifndef HTTPC_H
#define HTTPC_H

#include <3ds.h>

Result _HTTPC_Initialize(Handle* handle);
Result _HTTPC_CreateContext(Handle* handle, char* url, Handle* contextHandle);
Result _HTTPC_AddRequestHeaderField(Handle* handle, Handle contextHandle, char* name, char* value);
Result _HTTPC_GetResponseStatusCode(Handle* handle, Handle contextHandle, u32* out, u64 delay);
Result _HTTPC_GetResponseHeader(Handle* handle, Handle contextHandle, char* name, char* value, u32 valuebuf_maxsize);
Result _HTTPC_InitializeConnectionSession(Handle* handle, Handle contextHandle);
Result _HTTPC_SetProxyDefault(Handle* handle, Handle contextHandle);
Result _HTTPC_CloseContext(Handle* handle, Handle contextHandle);
Result _HTTPC_BeginRequest(Handle* handle, Handle contextHandle);
Result _HTTPC_ReceiveData(Handle* handle, Handle contextHandle, u8* buffer, u32 size);
Result _HTTPC_GetDownloadSizeState(Handle* handle, Handle contextHandle, u32* totalSize);

#endif