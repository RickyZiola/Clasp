#ifndef FILE_INFO_H
#define FILE_INFO_H

#include "../dynabuf/dynabuf.h"
 
DynamicBuffer(char *) *loaded_files;

void setup_files(void);

#endif