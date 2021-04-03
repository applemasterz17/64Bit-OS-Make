#include "Types.h"
#include "Keyboard.h"

void kPrintString(int iX, int iY, const char* pcString);

///////////////////////////////////////////////////////////////////////////
//                              Main                                     //
///////////////////////////////////////////////////////////////////////////
void Main(void)
{
    char vcTemp[2] = {0, };
    BYTE bFlags;
    BYTE bTemp; 
    int i = 0;

    kPrintString(0, 10, "Switch To IA-32e Mode Success~!!");
    kPrintString(0, 11, "IA-32e C Language Kernel Start..............[Pass]");
    kPrintString(0, 12, "Keyboard Activate...........................[    ]");

    // 키보드를 활성화
    if(kActivateKeyboard() == TRUE)
    {
        kPrintString(45, 12, "Pass");
        kChangeKeyboardLED(FALSE, FALSE, FALSE);
    }
    else
    {
        kPrintString(45, 12, "Fail");
        while(1);
    }

    while(1)
    {
        // 출력버퍼가 차 있으면 스캔 코드를 읽을 수 있음
        if(kIsOutputBufferFull() == TRUE)
        {
            // 스캔 코드를 받아온다 
            bTemp = kGetKeyboardScanCode();

            // 스캔 코드를 아스키 코드로 변환한다 
            if(kConvertScanCodeToASCIICode(bTemp, &(vcTemp[0]), &bFlags) == TRUE)
            {
                if(bFlags & KEY_FLAGS_DOWN)
                {
                    kPrintString(i++, 13, vcTemp);
                }
            }
        }
    }


}

///////////////////////////////////////////////////////////////////////////
//                              Function                                 //
///////////////////////////////////////////////////////////////////////////
void kPrintString(int iX, int iY, const char* pcString)
{
    CHARACTER* pstScreen = (CHARACTER *) 0xB8000;
    int i;

    pstScreen += (iY * 80) + iX;

    for(i = 0; pcString[i] != 0; i++)
    {
        pstScreen[i].bCharactor = pcString[i];
    }
}