#ifndef UPDATER_H
#define UPDATER_H

#include <3ds.h>

unsigned short ccitt16(unsigned char *data, unsigned int length);
Result update(u8* firmver, u32* decompressed_size);

#endif