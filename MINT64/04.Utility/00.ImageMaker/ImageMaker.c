#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BYTESOFSECTOR 512

int AdjustInSectorSize(int fd, int iSourceSize);
void WriteKernelInformation(int iTargetFd, int iKernelSectorCount);
int CopyFile(int iSourceFd, int iTargetFd);

int main(int argc, char *argv[])
{
    int iSourceFd;
    int iTargetFd;
    int iBootLoaderSize;
    int iKernel32SectorCount;
    int iSourceSize;

    // Check 3 Options
    if (argc < 3)
    {
        fprintf(stderr, "[Error] ImageMaker BootLoader.bin Kernel32.bin\n");
        exit(-1);
    }

    if ((iTargetFd = open("Disk.img", O_RDWR | O_CREAT | O_TRUNC | S_IREAD | S_IWRITE)) == -1)
    {
    }

    return 0;
}