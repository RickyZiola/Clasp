#include "fileinfo.h"

void setup_files(void) {
    loaded_files = newDynamicBuf(char *);
    initDynamicBuf(char *, loaded_files);
}