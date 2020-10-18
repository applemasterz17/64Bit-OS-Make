#include "Types.h"
#include "Page.h"
#include "ModeSwitch.h"

void kPrintString(int iX, int iY, const char *pcString);    // 메시지 출력 함수 (CUI)
BOOL kInitializeKernel64Area(void);                         // IA-32e 커널이 올라갈 공간을 0으로 초기화 하는 함수
BOOL kIsMemoryEnough(void);                                 // 보호모드 설치된 메모리 크기 검사 함수 (4GB)


///////////////////////////////////////////////////////////////////////////
//                              메인                                      //
///////////////////////////////////////////////////////////////////////////
void Main(void)
{
    DWORD i;
    DWORD dwEAX, dwEBX, dwECX, dwEDX;
    char vcVendorString[13] = {0, };

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

    // [3] Create IA-32e Kernel Page Table 
    kPrintString(0, 6, "IA-32e Page Tables Initialize...............[    ]");
    kInitializePageTables();
    kPrintString(45, 6, "Pass");

    // [4] Read Process Maker's Information
    kReadCPUID(0x00, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    *(DWORD *)vcVendorString = dwEBX;
    *((DWORD *)vcVendorString + 1) = dwEDX;
    *((DWORD *)vcVendorString + 2) = dwECX;
    kPrintString(0, 7, "Processor Vendor String.....................[           ]");
    kPrintString(45, 7, vcVendorString);

    // [5] Is Able To Change 64bit ?? 
    kReadCPUID(0x80000001, &dwEAX, &dwEBX, &dwECX, &dwEDX);
    kPrintString(0, 8, "64bit Mode Support Check....................[    ]");
    if(dwEDX & (1 << 29))
    {
        kPrintString(45, 8, "Pass");
    }
    else
    {
        kPrintString(45, 8, "Fail");
        kPrintString(0, 9, "This processor does not support 64bit mode~!!");
        while(1)
            ;
    }

    // [6] Change IA-32e Mode
    kPrintString(0, 9, "Switch To IA-32e Mode");
    // kSwitchAndExecute64bitKernel();

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
        pstScreen[i].bAttribute = 3;
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