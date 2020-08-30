#include "Types.h"

void kPrintString(int iX, int iY, const char *pcString);
BOOL kInitializeKernel64Area(void);
BOOL kIsMemoryEnough(void);

// Lower 32 Bit
#define PAGE_FLAGS_P 0x00000001   // Present
#define PAGE_FLAGS_RW 0x00000002  // Read/Write
#define PAGE_FLAGS_US 0x00000004  // User/Supervisor
#define PAGE_FLAGS_PWT 0x00000008 // Page Level Write-through
#define PAGE_FLAGS_PCD 0x00000010 // Page Level Cache Disable
#define PAGE_FLAGS_A 0x00000020   // Accessed
#define PAGE_FLAGS_D 0x00000040   // Dirty
#define PAGE_FLAGS_PS 0x00000080  // Page Size
#define PAGE_FLAGS_G 0x00000100   // Global
#define PAGE_FLAGS_PAT 0x0001000  // Page Attribute Table Index

// Upper 32 Bit
#define PAGE_FLAGS_EXB 0x80000000 // Excute Disable

// ETC
#define PAGE_FLAGS_DEFAULT (PAGE_FLAGS_P | PAGE_FLAGS_RW)

typedef struct pageTableEntryStruct
{
    // Lower Base Address + G + PAT + D + A + PCD + PWT + U/S + R/W + P
    DWORD dwAttributeAndLowerBaseAddress;
    // Upper Base Address +
    DWORD dwUpperBaseAddressAndEXB;
} PML4TENTRY, PDPTENTRY, PDENTRY, PTENTRY;

// Main Function
void Main(void)
{

    DWORD i;

    kPrintString(0, 3, "C Language Kernel Start.....................[Pass]");

    ///////////////////////////////////////
    // [1] Check Minimum Memory Size
    ///////////////////////////////////////
    kPrintString(0, 4, "Minimum Memory Size Check...................[    ]");
    if (kIsMemoryEnough() == FALSE)
    {
        kPrintString(45, 4, "Fail");
        kPrintString(0, 5, "Not Enough Memory!! Requires Over 64MB Memory!!");
        while (1)
            ;
    }
    else
    {
        kPrintString(45, 4, "Pass");
    }

    ///////////////////////////////////////
    // [2] Initialize IA-32e Kernel Area
    ///////////////////////////////////////
    kPrintString(0, 5, "IA-32e Kernel Area Initialize...............[    ]");
    if (kInitializeKernel64Area() == FALSE)
    {
        kPrintString(45, 5, "Fail");
        kPrintString(0, 6, "Kernel Area Initialization Fail!!");
        while (1)
            ;
    }
    kPrintString(45, 5, "Pass");

    while (1)
        ;
}

// String Printing Function
void kPrintString(int iX, int iY, const char *pcString)
{
    CHARACTER *pstScreen = (CHARACTER *)0xB8000;
    int i;

    pstScreen += (iY * 80) + iX;
    for (i = 0; pcString[i] != 0; i++)
    {
        pstScreen[i].bCharactor = pcString[i];
    }
}

BOOL kInitializeKernel64Area(void)
{
    DWORD *pdwCurrentAddress;
    pdwCurrentAddress = (DWORD *)0x100000;

    while ((DWORD)pdwCurrentAddress < 0x600000)
    {
        *pdwCurrentAddress = 0x00;

        if (*pdwCurrentAddress != 0)
        {
            return FALSE;
        }

        pdwCurrentAddress++;
    }
    return TRUE;
}

// 커널 메모리를 확인하는 프로그램
BOOL kIsMemoryEnough(void)
{
    DWORD *pdwCurrentAddress;
    pdwCurrentAddress = (DWORD *)0x100000;

    while ((DWORD)pdwCurrentAddress < 0x4000000)
    {
        *pdwCurrentAddress = 0x12345678;

        if (*pdwCurrentAddress != 0x12345678)
        {
            return FALSE;
        }

        pdwCurrentAddress += (0x100000 / 4);
    }
    return TRUE;
}