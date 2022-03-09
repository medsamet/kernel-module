
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
 

#define FIL_CM_ID       _IOW('a','a',int32_t*)
#define FIL_MSG_TYP     _IOW('b','b',int32_t*)
#define FIL_DAT_TYP     _IOW('c','c',int32_t*)
#define FIL_CHAN_ID     _IOW('d','d',int32_t*)
#define FIL_RESET       _IOW('e','e',int32_t*)
 
int main()
{
        int fd;
        int32_t number = 0;

        printf("***********************************************\n");
        printf("*******ioctl tecmp  Linux Driver*******\n");
 
        printf("\nOpening Driver\n");
        fd = open("/dev/tecmp", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
        printf("Enter the Value to send\n");
        scanf("%d",&number);
        if (number == -1) {
            ioctl(fd, FIL_RESET, (int32_t*) &number);
        } else {
        printf("Writing Value to Driver cmd = %ld\n", FIL_DAT_TYP);
        ioctl(fd, FIL_DAT_TYP, (int32_t*) &number);
        }
 

        close(fd);
}