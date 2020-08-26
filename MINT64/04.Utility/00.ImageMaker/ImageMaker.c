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
        fprintf(stderr, "[ERROR] ImageMaker BootLoader.bin Kernel32.bin\n");
        exit(-1);
    }

    // Create Disk.img File
    if ((iTargetFd = open("Disk.img", O_RDWR | O_CREAT | O_TRUNC | S_IREAD | S_IWRITE)) == -1)
    {
        fprintf(stderr, "[ERROR] Disk.img open fail.\n");
        exit(-1);
    }

    //--------------------------------------------------------------------------
    // Open BootLoader.bin, Copy All Data to Disk.img
    //--------------------------------------------------------------------------
    printf("[INFO] Copy boot loader to image file\n");
    if ((iSourceFd = open(argv[1], O_RDONLY)) == -1)
    {
        fprintf(stderr, "[ERROR] %s open fail\n", argv[1]);
        exit(-1);
    }
    iSourceSize = CopyFile(iSourceFd, iTargetFd);
    close(iSourceFd);

    // Fill Remain Sector, 512 align
    iBootLoaderSize = AdjustInSectorSize(iTargetFd, iSourceSize);
    printf("[INFO] %s size = [%d] and sector count = [%d]\n", argv[1], iSourceSize, iBootLoaderSize);

    //--------------------------------------------------------------------------
    // Open Kernel32.bin, Copy All Data to Disk.img
    //--------------------------------------------------------------------------
    printf("[INFO] Copy protected mode kernel to image file\n");
    if ((iSourceFd = open(argv[2], O_RDONLY)) == -1)
    {
        fprintf(stderr, "[ERROR] %s open fail\n", argv[2]);
        exit(-1);
    }
    iSourceSize = CopyFile(iSourceFd, iTargetFd);
    close(iSourceFd);

    // Fill Remain Sector, 512 align
    iKernel32SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
    printf("[INFO] %s size = [%d] and sector count = [%d]\n", argv[1], iSourceSize, iKernel32SectorCount);

    //--------------------------------------------------------------------------
    // Renewal Kernel Information in Disk.img
    //--------------------------------------------------------------------------
    printf("[INFO] Start to write kernel information\n");
    WriteKernelInformation(iTargetFd, iKernel32SectorCount);
    printf("[INFO] Image file create complete\n");

    close(iTargetFd);

    return 0;
}

// int AdjustInSectorSize(int iFd, int iSourceSize)
// {
//     return iSectorCount;
// }

// void WriteKernelInformation(int iTargetFd, int iKernelSectorCount)
// {
// }

// int CopyFile(int iSourceFd, int iTargetFd)
// {
//     return iSourceFileSize;
// }