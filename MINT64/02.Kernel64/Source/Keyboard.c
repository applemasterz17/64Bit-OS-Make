#include "Types.h"

// 출력 버퍼(포트 0x60)에 수신된 데이터가 있는지 여부를 반환
BOOL kIsOutputBufferFull(void)
{
    if(kInPortByte(0x64) & 0x01)
    {
        return TRUE;
    }
    return FALSE;
}

// 입력 버퍼(포트 0x64)에 프로세서가 쓴 데이터가 남아있는지 여부를 반환
BOOL kIsInputBufferFull(void)
{
    if(kInPortByte(0x64) & 0x02)
    {
        return TRUE;
    }
    return FALSE;
}

// 키보드를 활성화
BOOL kActivateKeyboard(void)
{
    int i;
    int j;

    kOutPortByte(0x64, 0xAE);

    for(i=0; i<0xFFFF; i++)
    {
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    kOutPortByte(0x60, 0xF4);

    for(j=0; j<100; j++)
    {
        for(i=0; i<0xFFFF; i++)
        {
            if(kIsOutputBufferFull() == TRUE)
            {
                break;
            }
        }

        if(kInPortByte(0x60) == 0xFA)
        {
            return TRUE;
        }
    }
    return FALSE;
}

