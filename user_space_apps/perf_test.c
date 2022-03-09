#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

//#include <sys/types.h>
//#include <sys/stat.h>

static inline long long int getTimeStamp_us();

//
//#include <unistd.h>
char readbuf[1536] = {0};
char* readptr;
char test1[] = {0x00, 0x80, 0x00, 0x00, 0x02, 0x03, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0F, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x47, 0x48, 0x50 };

char test2[] = {0x00, 0x30, 0x00, 0x01, 0x02, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0D, 0x23, 0x45, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x67, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05};

char test3[] = {0x00, 0x40, 0x00, 0x02, 0x02, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0E, 0x23, 0x45, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x67, 0x89, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x01, 0x23, 0x45, 0x67, 0x00, 0x0D, 0x00, 0x00};

char test4[] = {0x00, 0x40, 0x00, 0x03, 0x02, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x23, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03};

char test5[] = {0x00, 0x40, 0x00, 0x04, 0x02, 0x03, 0x00, 0x08, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x10, 0x23, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x45, 0x67, 0x89, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x23, 0x45, 0x67,\
                 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x49, 0x45, 0x67, 0x89, 0x01, 0x23, \
                 0x45, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x00, 0x05, 0x12, 0x34, 0x45, 0x46, 0x47, 0x48, 0x50, 0x01, 0x23, 0x45, 0x67, 0x00, 0x0E, 0x00, 0x00, 0x05, 0x00, 0x04, 0x0A, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35};

pthread_t tid[2];

static inline long long int getTimeStamp_us()
{
    struct timeval timer_usec; 
    long long int timestamp_usec; /* timestamp in microsecond */
    if (!gettimeofday(&timer_usec, NULL)) {
        return ((long long int) timer_usec.tv_sec) * 1000000ll + (long long int) timer_usec.tv_usec;
    }
  return -1;
}

void* readFromDrv(void *arg)
{
    ssize_t count = 0;
    int repeat = 50000;
    int fd = (int)arg;
    while (repeat > 0) {
        count = read(fd, readptr, 1536);
        if (count != 0) {
            printf (" %d -read %ld bytes at %lld\n", 5000 - repeat, count, getTimeStamp_us());
        }
        repeat--;
    }
}

void* writeIntoDrv(void *arg)
{
    ssize_t count = 0;
    int repeat = 10000;
    int fd = (int)arg;
    while (repeat > 0) {
        count = write(fd, test1, sizeof(test1));
        if (count != 0) {
            printf ("%d- write %ld bytes at %lld\n", 5000-repeat, count, getTimeStamp_us());
        }
        count = write(fd, test2, sizeof(test2));
        if (count != 0) {
            printf ("write %ld bytes at %lld\n", count, getTimeStamp_us());
        }
        count = write(fd, test3, sizeof(test3));
        if (count != 0) {
            printf ("write %ld bytes at %lld\n", count, getTimeStamp_us());
        }
        count = write(fd, test4, sizeof(test4));
        if (count != 0) {
            printf ("write %ld bytes at %lld\n", count, getTimeStamp_us());
        }
        count = write(fd, test5, sizeof(test5));
        if (count != 0) {
            printf ("write %ld bytes at %lld\n", count, getTimeStamp_us());
        }
        repeat--;
    }
}

int main(void)
{
    int err = 0;
    readptr = readbuf;
    int fd = -1;
    fd = open("/dev/tecmp_primatec", O_RDWR);
    if(fd < 0) {
        printf("Cannot open device file...\n");
        return -1;
    }
    err = pthread_create(&(tid[0]), NULL, &writeIntoDrv, (void*)fd);
    if (err == 0) {
        err = pthread_create(&(tid[1]), NULL, &readFromDrv, (void*)fd);
    }
    pthread_join( tid[0], NULL);
    pthread_join( tid[1], NULL);
    close(fd);
    return err;
}