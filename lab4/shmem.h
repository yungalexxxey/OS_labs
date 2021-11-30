#ifndef INC_4_LAB_SHRMEM_H
#define INC_4_LAB_SHRMEM_H

#include <fcntl.h>

const size_t map_size = 4096;

const char * BackingFile1 = "os_lab4.1.back";
const char * SemaphoreName1 = "os_lab4.1.semaphore";
const char * BackingFile2 = "os_lab4.2.back";
const char * SemaphoreName2 = "os_lab4.2.semaphore";

unsigned AccessPerms = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;

#endif //INC_4_LAB_SHRMEM_H
