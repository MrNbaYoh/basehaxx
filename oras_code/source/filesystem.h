#ifndef FS_H
#define FS_H

#include <3ds.h>

Result _FSUSER_OpenFile(Handle* handle, Handle* out, FS_Archive archive, FS_Path fileLowPath, u32 openflags, u32 attributes);
Result _FSUSER_OpenArchive(Handle *handle, FS_Archive *archive);
Result _FSFILE_Close(Handle handle);
Result _FSFILE_Read(Handle handle, u32 *bytesRead, u64 offset, u32 *buffer, u32 size);
Result _FSFILE_GetSize(Handle handle, u64 *size);
Result _FSFILE_Write(Handle handle, u32 *bytesWritten, u64 offset, u32 *data, u32 size, u32 flushFlags);
Result _FSUSER_DeleteFile(Handle *handle, FS_Archive archive, FS_Path fileLowPath);
Result _FSUSER_ControlArchive(Handle *handle, FS_Archive archive, FS_ArchiveAction action, void* input, u32 inputSize, void* output, u32 outputSize);
Result _FSUSER_CloseArchive(Handle* handle, FS_Archive* archive);

#endif