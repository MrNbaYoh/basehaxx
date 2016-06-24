#include "hid.h"
#include "imports.h"

u32 hidKey()
{
	return *hidKeys;
}

u32 hidWaitKey()
{
    u32 old = *hidKeys;
    while(true)
    {
        svcSleepThread(1000*1000);
        u32 state = *hidKeys;
        if (old ^ state)
            return state;
    }
}