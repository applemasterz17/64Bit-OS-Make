#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
// #include <sys/io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BYTESOFSECTOR 512

int AdjustInSectorSize(int fd, int iSourceSize);
void WriteKernelInformation(int iTargetFd, int iTotalKernelSectorCount, int iKernel32SectorCount);
int CopyFile(int iSourceFd, int iTargetFd);

int main(int argc, char *argv[])
{
    int iSourceFd; // 이미 생성된 .bin 파일의 FD  
    int iTargetFd; // Disk.img 의 FD  
    int iBootLoaderSize; // 부트 로더의 크기 
    int iKernel32SectorCount; // 32비트 커널의 섹터 수
    int iKernel64SectorCount;
    int iSourceSize; // 복사 할때, src 에서 읽어온 크기 

    //--------------------------------------------------------------------------
    // Check Argument, Create "Disk.img"
    //--------------------------------------------------------------------------
    if (argc < 4)
    {
        fprintf(stderr, "[ERROR] ImageMaker BootLoader.bin Kernel32.bin Kernel64.bin\n");
        exit(-1);
    }

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
    printf("[INFO] %s size = [%d] and sector count = [%d]\n", argv[2], iSourceSize, iKernel32SectorCount);

    //--------------------------------------------------------------------------
    // Open Kernel64.bin, Copy All Data to Disk.img
    //--------------------------------------------------------------------------
    printf("[INFO] Copy protected mode kernel to image file\n");
    if ((iSourceFd = open(argv[3], O_RDONLY)) == -1)
    {
        fprintf(stderr, "[ERROR] %s open fail\n", argv[3]);
        exit(-1);
    }
    iSourceSize = CopyFile(iSourceFd, iTargetFd);
    close(iSourceFd);

    // Fill Remain Sector, 512 align
    iKernel64SectorCount = AdjustInSectorSize(iTargetFd, iSourceSize);
    printf("[INFO] %s size = [%d] and sector count = [%d]\n", argv[3], iSourceSize, iKernel64SectorCount);

    //--------------------------------------------------------------------------
    // Renewal Kernel Information in Disk.img (Update Sector Count)
    //--------------------------------------------------------------------------
    printf("[INFO] Start to write kernel information\n");
    WriteKernelInformation(iTargetFd, iKernel32SectorCount + iKernel64SectorCount, iKernel32SectorCount);
    printf("[INFO] Image file create complete\n");

    close(iTargetFd);

    return 0;
}

int AdjustInSectorSize(int iFd, int iSourceSize)
{
    int i;
    int iAdjustSizeToSector;
    char cCh;
    int iSectorCount;

    iAdjustSizeToSector = iSourceSize % BYTESOFSECTOR;
    cCh = 0x00;

    if (iAdjustSizeToSector != 0)
    {
        iAdjustSizeToSector = 512 - iAdjustSizeToSector;
        printf("[INFO] File size [%lu] and fill [%u] byte\n", iSourceSize, iAdjustSizeToSector);
        for (i = 0; i < iAdjustSizeToSector; i++)
        {
            write(iFd, &cCh, 1);
        }
    }
    else
    {
        printf("[INFO] File size is aligned 512 byte\n");
    }

    // Return Total Sector Number
    iSectorCount = (iSourceSize + iAdjustSizeToSector) / BYTESOFSECTOR;
    return iSectorCount;
}

void WriteKernelInformation(int iTargetFd, int iTotalKernelSectorCount, int iKernel32SectorCount)
{
    unsigned short usData;
    long lPosition;

    // Change Current TargetFd's Location
    lPosition = lseek(iTargetFd, (off_t)5, SEEK_SET);
    if (lPosition == -1)
    {
        fprintf(stderr, "lseek fail. Return value = %d, errno = %d, %d\n", lPosition, errno, SEEK_SET);
        exit(-1);
    }

    usData = (unsigned short)iTotalKernelSectorCount;
    write(iTargetFd, &usData, 2);
    usData = (unsigned short)iKernel32SectorCount;
    write(iTargetFd, &usData, 2);

    printf("[INFO] Total sector count except boot loader [%d]\n", iTotalKernelSectorCount);
    printf("[INFO] Total sector count of protected mode kernel [%d]\n", iKernel32SectorCount);
}

int CopyFile(int iSourceFd, int iTargetFd)
{
    int iSourceFileSize;
    int iRead;
    int iWrite;
    char vcBuffer[BYTESOFSECTOR];

    iSourceFileSize = 0;
    while (1)
    {
        iRead = read(iSourceFd, vcBuffer, sizeof(vcBuffer));
        iWrite = write(iTargetFd, vcBuffer, iRead);

        if (iRead != iWrite)
        {
            fprintf(stderr, "[ERROR] iRead != iWrite.. \n");
            exit(-1);
        }
        iSourceFileSize += iRead;

        // 만약 src 에서 읽어온 크기가 512 랑 다르다면, 
        // 한 섹터(512) 보다 적게 읽어온 것이므로 반복문 탈출 
        if (iRead != sizeof(vcBuffer))
        {
            break;
        }
    }
    // 총 읽어온 크기를 리턴 
    return iSourceFileSize;
}