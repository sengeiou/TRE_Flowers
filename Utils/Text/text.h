#ifndef _TEXT_H
#define _TEXT_H

#include <string.h>
#include <stdbool.h>
#include "fatfs.h"

enum
{
	TEXT_READ_PREV,
	TEXT_READ_NEXT,
};

void text_read(uint8_t dir);
bool text_load(const char* path, FSIZE_t ofs);
void text_close(void);
bool text_init(void);

#endif

