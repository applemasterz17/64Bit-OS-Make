#include "Types.h"

// 메시지 출력 함수 (CUI)
void kPrintString(int iX, int iY, const char *pcString);
// IA-32e 커널이 올라갈 공간을 0으로 초기화 하는 함수
BOOL kInitializeKernel64Area(void);
// 보호모드 설치된 메모리 크기 검사 함수 (4GB)
BOOL kIsMemoryEnough(void);

///////////////////////////////////////////////////////////////////////////
//                              페이징에 사용                               //
///////////////////////////////////////////////////////////////////////////

// 페이지 엔트리 : 하위 32비트 용 속성 필드
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

//페이지 엔트리 : 상위 32비트 용 속성 필드
#define PAGE_FLAGS_EXB 0x80000000 // Excute Disable

// 페이지 엔트리 : 기타
#define PAGE_FLAGS_DEFAULT (PAGE_FLAGS_P | PAGE_FLAGS_RW)

// 페이지 엔트리 : 자료구조
typedef struct pageTableEntryStruct
{
    DWORD dwAttributeAndLowerBaseAddress; // Addr + G + PAT + D + A + PCD + PWT + U/S + R/W + P
    DWORD dwUpperBaseAddressAndEXB;       // Addr + EXB
} PML4TENTRY, PDPTENTRY, PDENTRY, PTENTRY;

///////////////////////////////////////////////////////////////////////////
//                              메인 함수                                  //
///////////////////////////////////////////////////////////////////////////
void Main(void)
{

    DWORD i;

    kPrintString(0, 3, "C Language Kernel Start.....................[Pass]");

    // [1] Check Minimum Memory Size : 설치된 메모리 크기 검사 (4GB)
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

    // [2] Initialize IA-32e Kernel Area : IA-32e 영역 초기화
    kPrintString(0, 5, "IA-32e Kernel Area Initialize...............[    ]");
    if (kInitializeKernel64Area() == FALSE)
    {
        kPrintString(45, 5, "Fail");
        kPrintString(0, 6, "Kernel Area Initialization Fail!!");
        while (1)
            ;
    }
    else
    {
        kPrintString(45, 5, "Pass");
    }

    while (1)
        ;
}

///////////////////////////////////////////////////////////////////////////
//                              함수 영역                                  //
///////////////////////////////////////////////////////////////////////////
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