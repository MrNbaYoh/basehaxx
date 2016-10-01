#include "updater.h"
#include "imports.h"
#include "httpc.h"
#include "utils.h"
#include "xprintf.h"
#include "filesystem.h"
#include "blz.h"
#include "screen_draw.h"

#define CRC_POLY 0x1021

Result get_redirect(char *url, char *out, size_t out_size, char *user_agent)
{
    Result ret;

    Handle context;
    ret = _HTTPC_CreateContext(&httpcHandle, url, &context);
    if(ret) return ret;

    Handle httpcHandle2;
    ret = _srvGetServiceHandle(&httpcHandle2, "http:C");
    if(ret) return ret;

    ret = _HTTPC_InitializeConnectionSession(&httpcHandle2, context);
    if(ret) return ret;
	
    ret = _HTTPC_SetProxyDefault(&httpcHandle2, context);
    if(ret) return ret;

    ret = _HTTPC_AddRequestHeaderField(&httpcHandle2, context, "User-Agent", user_agent);
    if(!ret) ret = _HTTPC_BeginRequest(&httpcHandle2, context);

    if(ret)
    {
        _HTTPC_CloseContext(&httpcHandle2, context);
        return ret;
    }

    ret = _HTTPC_GetResponseHeader(&httpcHandle2, context, "Location", out, out_size);
    _HTTPC_CloseContext(&httpcHandle2, context);

    return ret;
}

Result download_file(Handle handle, Handle context, u8* buffer, u32* size)
{
    Result ret;

    ret = _HTTPC_BeginRequest(&handle, context);
    if (R_FAILED(ret)) return ret;

    u32 status_code = 0;
    ret = _HTTPC_GetResponseStatusCode(&handle, context, &status_code, 0);
    if (R_FAILED(ret)) return ret;

    if (status_code != 200) return -1;

    u32 sz = 0;
    ret = _HTTPC_GetDownloadSizeState(&handle, context, &sz);
    if (R_FAILED(ret)) return ret;

    memset(buffer, 0, sz);

    ret = _HTTPC_ReceiveData(&handle, context, buffer, sz);
    if (R_FAILED(ret)) return ret;

    if(size) *size = sz;
	
    return 0;
}

unsigned short ccitt16(unsigned char *data, unsigned int length)
{
    unsigned short crc = 0xFFFF;

    for (unsigned int i = 0; i < length; i++)
    {
        crc ^= (unsigned short)data[i] << 8;

        for (int j = 0; j < 8; j++)
            if (crc & 0x8000)
                crc = crc << 1 ^ CRC_POLY;
            else
                crc = crc << 1;
    }

    return crc;
}

Result update(u8* firmver, u32* decompressed_size)
{
	u32 size;

	u8* buffer = LINEAR_BUFFER;
	char* in_url = (char*)&LINEAR_BUFFER[0x0017E900];
	char* out_url = in_url + 512;
	
	xsprintf(in_url, "http://smea.mtheall.com/get_payload.php?version=%s-%d-%d-%d-%d-%s", firmver[0] ? "NEW" : "OLD", firmver[1], firmver[2], firmver[3], firmver[4], regions[firmver[5]]); 

	char user_agent[] = "basehaxx_updater";
	Result ret = get_redirect(in_url, out_url, 512, user_agent);
	/*xsprintf(in_url, "%X\0", ret);
	centerString(&LINEAR_BUFFER[0x00100000], in_url, 24, 400);*/
	if(ret) return ret;
	
	Handle context;
	ret = _HTTPC_CreateContext(&httpcHandle, out_url, &context);
	if(ret) return ret;
	
	Handle httpcHandle2;
	ret = _srvGetServiceHandle(&httpcHandle2, "http:C");
    if(ret) return ret;

    ret = _HTTPC_InitializeConnectionSession(&httpcHandle2, context);
    if(ret) return ret;

    ret = _HTTPC_SetProxyDefault(&httpcHandle2, context);
    if(ret) return ret;
	
    ret = download_file(httpcHandle2, context, buffer, &size);
    if(ret) return ret;
	
	*decompressed_size = size;
	memcpy(LINEAR_BUFFER, buffer, size);
	buffer = BLZ_Code(buffer, size, (unsigned int*)&size, BLZ_NORMAL);
	
	FS_Archive save_archive = 0;
	ret = _FSUSER_OpenArchive(fsHandle, &save_archive, ARCHIVE_SAVEDATA, (FS_Path){PATH_EMPTY, 1, (u8*)""});
    if(ret) return ret;
	
	Handle file;
	ret = _FSUSER_OpenFile(fsHandle, &file, save_archive, _fsMakePath(PATH_ASCII, "/main"), FS_OPEN_READ | FS_OPEN_WRITE, 0);
	if(ret) return ret;
	
	u32 bytes = 0;
	ret = _FSFILE_Write(file, &bytes, PICDATA_SAVE_OFFSET + HAX_PAYLOAD_OFFSET, (u32*)buffer, size, FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
	if(ret) return ret;
	
	u8* picdata_temp_buf = &LINEAR_BUFFER[0x0017E900];
	ret = _FSFILE_Read(file, &bytes, PICDATA_SAVE_OFFSET, (u32*)picdata_temp_buf, PICDATA_SIZE);
	if(ret) return ret;
	
	u16 chk = ccitt16(picdata_temp_buf, PICDATA_SIZE);
	ret = _FSFILE_Write(file, &bytes, PICDATA_CHK_OFFSET, (u32*)&chk, 2, FS_WRITE_FLUSH | FS_WRITE_UPDATE_TIME);
	if(ret) return ret;
	
	ret = _FSFILE_Close(file);
    if(ret) return ret;
	
	ret = _FSUSER_ControlArchive(fsHandle, save_archive, ARCHIVE_ACTION_COMMIT_SAVE_DATA, NULL, 0, NULL, 0);
    if(ret) return ret;

    ret = _FSUSER_CloseArchive(fsHandle, save_archive);
    return ret;
}