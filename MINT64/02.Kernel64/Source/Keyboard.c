#include "Types.h"
#include "AssemblyUtility.h"
#include "Keyboard.h"

////////////////////////////////////////////////////////
// 키보드 컨트롤러와 키보드 제어에 관련된 함수 
////////////////////////////////////////////////////////

// 출력 버퍼(포트 0x60)에 수신된 데이터가 있는지 여부를 반환
// 버퍼에서 값을 가져올때 사용한다 
BOOL kIsOutputBufferFull(void)
{
    if(kInPortByte(0x64) & 0x01)
    {
        return TRUE;
    }
    return FALSE;
}

// 입력 버퍼(포트 0x60)에 프로세서가 쓴 데이터가 남아있는지 여부를 반환
// 버퍼에 값을 쓸때(키보드 입력전) 사용, FALSE 리턴시에 입력에 들어간다
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

    // 0x64 컨트롤 레지스터에 0xAE 커맨드를 전달하여 키보드 컨트롤러에 활성화 요청 
    kOutPortByte(0x64, 0xAE);

    // 버퍼에 데이터가 남아있나 확인
    // 비어있다면 반복문 탈출하고 버퍼에 데이터 쓰기
    for(i=0; i<0xFFFF; i++)
    {
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }
    // 입력 버퍼에 0xF4 커맨드를 전달하여 키보드에 직접 활성화 요청 
    kOutPortByte(0x60, 0xF4);

    // ACK 도착전 이미 출력 버퍼에 값이 있을수도 있으니 100회 진행한다
    for(j=0; j<100; j++)
    {
        //버퍼에 데이터가 도착했나 확인
        // 데이터가 들어있으면 반복문 탈출하고 데이터를 읽어온다
        for(i=0; i<0xFFFF; i++)
        {
            if(kIsOutputBufferFull() == TRUE)
            {
                break;
            }
        }
        // 출력 버퍼에서 읽은데이터가 ACK(0xFA) 이면 키보드 활성화 성공 (TRUE)
        if(kInPortByte(0x60) == 0xFA)
        {
            return TRUE;
        }
    }
    return FALSE;
}

// 키보드 컨트롤에서 키 값(스캔코드)를 읽는 코드 
BYTE kGetKeyboardScanCode(void)
{
    // 출력 버퍼에 데이터가 있을때까지 무한루프
    while(kIsOutputBufferFull() == FALSE)
    {
        ;
    }
    // 데이터가 생기면 키값 읽어서 리턴
    return kInPortByte(0x60);
}

// 키보드의 상태 LED를 제어하는 코드
// LED의 On/Off 상태를 파라미터로 넘겨받는다
BOOL kChangeKeyboardLED(BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn)
{
    int i, j;
    
    for(i=0; i<0xFFFF; i++)
    {
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    // LED 상태 전송 미리 알려주고, 가져갔는지 확인
    kOutPortByte(0x60, 0xED);
    for(i=0; i<0xFFFF; i++)
    {
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    // ACK 올때까지 대기
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
            break;
        }
    }
    if(j >= 100)
    {
        return FALSE;
    }

    // LED 상태 데이터 전송, 가져갔는지 확인
    kOutPortByte(0x60, (bCapsLockOn << 2) | (bNumLockOn << 1) | bScrollLockOn);
    for(i=0; i<0xFFFF; i++)
    {   
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }

    // ACK 올때까지 대기 
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
            break;
        }
    }
    if(j >= 100)
    {   
        return FALSE;
    }

    return TRUE;
}

// A20 게이트를 활성화 (키보드 컨트롤러 이용)
void kEnableA20Gate(void)
{
    BYTE bOutputPortData;
    int i;

    // 컨트롤 레지스터에 컨트롤러 출력 포트 값을 읽는 커맨드 전송 
    // 버퍼에 값 도착할때까지 대기 
    kOutPortByte(0x64, 0xD0);
    for(i = 0; i < 0xFFFF; i++)
    {
        if(kIsOutputBufferFull() == TRUE)
        {
            break;
        }
    }

    // 출력 버퍼에서 출력 포트 값을 읽어온뒤
    // 읽어온 값에 A20 게이트 활성화 비트를 켠다 
    bOutputPortData = kInPortByte(0x60);
    bOutputPortData != 0x02;

    // 입력 버퍼가 비어있다면 
    // 커맨드 레지스터에 출력 포트 설정 커맨드 전달하고
    // 입력 버퍼에 A20 게이트 비트가 1로 설정된 값을 전달
    for(i = 0; i < 0xFFFF; i++)
    {
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }
    kOutPortByte(0x64, 0xD1);
    kOutPortByte(0x60, bOutputPortData);
}

// 프로세서를 리셋 
void kReboot(void)
{
    int i;

    // 입력 버퍼가 비어있다면 
    // 커맨드 레지스터에 출력 포트 설정 커맨드 전달하고
    // 입력 버퍼에 0x00을 전달하여 프로세서를 리셋한다 
    for(i = 0; i < 0xFFFF; i++)
    {
        if(kIsInputBufferFull() == FALSE)
        {
            break;
        }
    }
    kOutPortByte(0x64, 0xD1);
    kOutPortByte(0x60, 0x00);

    while(1)
    {
        ;
    }
}

////////////////////////////////////////////////////////
// 키보드 컨트롤러와 키보드 제어에 관련된 함수 
////////////////////////////////////////////////////////

// 키보드 상태를 관리하는 키보드 매니저, 0 으로 초기화 
static KEYBOARDMANAGER gs_stKeyboardManager = {0, };

// 스캔 코드를 ASCII 코드로 변환하는 테이블
static KEYMAPPINGENTRY gs_vstKeyMappingTable[KEY_MAPPINGTABLEMAXCOUNT] =
{
    /* 0  */ {KEY_NONE, KEY_NONE},
    /* 1  */ {KEY_ESC, KEY_ESC},
    /* 2  */ {'1', '!'},
    /* 3  */ {'2', '@'},
    /* 4  */ {'3', '#'},
    /* 5  */ {'4', '$'},
    /* 6  */ {'5', '%'},
    /* 7  */ {'6', '^'},
    /* 8  */ {'7', '&'},
    /* 9  */ {'8', '*'},
    /* 10 */ {'9', '('},
    /* 11 */ {'0', ')'},
    /* 12 */ {'-', '_'},
    /* 13 */ {'=', '+'},
    /* 14 */ {KEY_BACKSPACE, KEY_BACKSPACE},
    /* 15 */ {KEY_TAB, KEY_TAB},
    /* 16 */ {'q', 'Q'},
    /* 17 */ {'w', 'W'},
    /* 18 */ {'e', 'E'},
    /* 19 */ {'r', 'R'},
    /* 20 */ {'t', 'T'},
    /* 21 */ {'y', 'Y'},
    /* 22 */ {'u', 'U'},
    /* 23 */ {'i', 'I'},
    /* 24 */ {'o', 'O'},
    /* 25 */ {'p', 'P'},
    /* 26 */ {'[', '{'},
    /* 27 */ {']', '}'},
    /* 28 */ {'\n', '\n'},
    /* 29 */ {KEY_CTRL, KEY_CTRL},
    /* 30 */ {'a', 'A'},
    /* 31 */ {'s', 'S'},
    /* 32 */ {'d', 'D'},
    /* 33 */ {'f', 'F'},
    /* 34 */ {'g', 'G'},
    /* 35 */ {'h', 'H'},
    /* 36 */ {'j', 'J'},
    /* 37 */ {'k', 'K'},
    /* 38 */ {'l', 'L'},
    /* 39 */ {';', ':'},
    /* 40 */ {'\'', '\"'},
    /* 41 */ {'`', '~'},
    /* 42 */ {KEY_LSHIFT, KEY_LSHIFT},
    /* 43 */ {'\\', '|'},
    /* 44 */ {'z', 'Z'},
    /* 45 */ {'x', 'X'},
    /* 46 */ {'c', 'C'},
    /* 47 */ {'v', 'V'},
    /* 48 */ {'b', 'B'},
    /* 49 */ {'n', 'N'},
    /* 50 */ {'m', 'M'},
    /* 51 */ {',', '<'},
    /* 52 */ {'.', '>'},
    /* 53 */ {'/', '?'},
    /* 54 */ {KEY_RSHIFT, KEY_RSHIFT},
    /* 55 */ {'*', '*'},
    /* 56 */ {KEY_LALT, KEY_LALT},
    /* 57 */ {' ', ' '},
    /* 58 */ {KEY_CAPSLOCK, KEY_CAPSLOCK},
    /* 59 */ {KEY_F1, KEY_F1},
    /* 60 */ {KEY_F2, KEY_F2},
    /* 61 */ {KEY_F3, KEY_F3},
    /* 62 */ {KEY_F4, KEY_F4},
    /* 63 */ {KEY_F5, KEY_F5},
    /* 64 */ {KEY_F6, KEY_F6},
    /* 65 */ {KEY_F7, KEY_F7},
    /* 66 */ {KEY_F8, KEY_F8},
    /* 67 */ {KEY_F9, KEY_F9},
    /* 68 */ {KEY_F10, KEY_F10},
    /* 69 */ {KEY_NUMLOCK, KEY_NUMLOCK},
    /* 70 */ {KEY_SCROLLLOCK, KEY_SCROLLLOCK},

    /* 71 */ {KEY_HOME, '7'},
    /* 72 */ {KEY_UP, '8'},
    /* 73 */ {KEY_PAGEUP, '9'},
    /* 74 */ {'-', '-'},
    /* 75 */ {KEY_LEFT, '4'},
    /* 76 */ {KEY_CENTER, '5'},
    /* 77 */ {KEY_RIGHT, '6'},
    /* 78 */ {'+', '+'},
    /* 79 */ {KEY_END, '1'},
    /* 80 */ {KEY_DOWN, '2'},
    /* 81 */ {KEY_PAGEDOWN, '3'},
    /* 82 */ {KEY_INS, '0'},
    /* 83 */ {KEY_DEL, '.'},
    /* 84 */ {KEY_NONE, KEY_NONE},
    /* 85 */ {KEY_NONE, KEY_NONE},
    /* 86 */ {KEY_NONE, KEY_NONE},
    /* 87 */ {KEY_F11, KEY_F11},
    /* 88 */ {KEY_F12, KEY_F12}
};

// 스캔 코드가 알파벳 범위인지 여부를 반환 
BOOL kIsAlphabetScanCode(BYTE bScanCode)
{
    // 변환 테이블을 참조하여 ASCII 값 'a' ~ 'z' 범위면 알파벳 범위
    if(('a' <= gs_vstKeyMappingTable[bScanCode].bNormalCode) &&
        (gs_vstKeyMappingTable[bScanCode].bNormalCode <= 'z'))
    {
        return TRUE;
    }
    return FALSE;
}

// 스캔 코드가 숫자/기호 범위인지 여부를 반환 
BOOL kIsNumberOrSymbolScanCode(BYTE bScanCode)
{
    // Down 코드값 2 ~ 53 범위고, 알파벳이 아니면 숫자/기호 범위 
    if((2 <= bScanCode) && (bScanCode <= 53) &&
        (kIsAlphabetScanCode(bScanCode) == FALSE))
    {
        return TRUE;
    }
    return FALSE;
}

// 스캔 코드가 넘버 패드 범위인지 여부를 반환 
BOOL kIsNumberPadScanCode(BYTE bScanCode)
{
    // Down 코드값 71 ~ 83 범위는 넘버패드 범위 
    if((71 <= bScanCode) && (bScanCode <= 83)) 
    {
        return TRUE;
    }
    return FALSE;
}

// 조합 키의 상태를 갱신하고 LED 상태도 동기화 한다 
void UpdateCombinationKeyStatusAndLED(BYTE bScanCode)
{
    BOOL bDown;
    BYTE bDownScanCode;
    BOOL bLEDStatusChanged = FALSE;

    // 눌림 또는 떨어짐 상태처리, 
    // 0x80(1000 0000) 과 AND 연산을 하면 Down 인지 Up 인지 구별 가능 
    // 최상위 비트(7)가 1이면 키가 떨어졌음을 의미하고, 0이면 눌림을 의미함
    if(bScanCode & 0x80)
    {
        bDown = FALSE;
        bDownScanCode = bScanCode & 0x7F; 

    }
    else
    {
        bDown = TRUE;
        bDownScanCode = bScanCode;
    }

    // 조합 키 검색
    // Shift 키의 스캔 코드(42 or 54)이면 Shift 키의 상태를 갱신
    if((bDownScanCode == 42) || (bDownScanCode == 54))
    {
        gs_stKeyboardManager.bShiftDown = bDown;
    }
    // Caps Lock 스캔 코드(58) 이면 Caps Lock의 상태 갱신하고 LED 상태 변경
    else if((bDownScanCode == 58) && (bDown == TRUE))
    {
        gs_stKeyboardManager.bCapsLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    // Num Lock 스캔 코드(69) 이면 Num Lock의 상태 갱신하고 LED 상태 변경 
    else if((bDownScanCode == 69) && (bDown == TRUE))
    {
        gs_stKeyboardManager.bNumLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }
    // Scroll Lock 스캔 코드(70) 이면 Scroll Lock의 상태를 갱신하고 LED 상태 변경 
    else if((bDownScanCode == 70) && (bDown == TRUE))
    {
        gs_stKeyboardManager.bScrollLockOn ^= TRUE;
        bLEDStatusChanged = TRUE;
    }

    // LED 상태가 변했다면 키보드로 커맨드를 전송하여 LED를 변경 
    if(bLEDStatusChanged == TRUE)
    {
        kChangeKeyboardLED(gs_stKeyboardManager.bCapsLockOn, gs_stKeyboardManager.bNumLockOn, 
                            gs_stKeyboardManager.bScrollLockOn);
    }
}

// 조합된 키 값을 사용해야하는지 여부를 반환 
// 인자로 Up 스캔 코드를 넘겨주면 Down 스캔 코드로 변환한뒤,
// 스캔 코드의 범위를 판단해  bUseCombinedKey 불리언 변수를 리턴한다
BOOL kIsUseCombinedCode(BYTE bScanCode)
{
    BYTE bDownScanCode;
    BOOL bUseCombinedKey = FALSE;

    // Up 스캔 코드를 Down 스캔 코드로 변경 
    // 0x7F 와 AND 연산을 하면 0x80 만큼 뺀것과 같음
    bDownScanCode = bScanCode & 0x7F;

    // 알파벳이면 Shift, CapsLock 영향 을 받음
    // Shift, CapsLock 둘중 하나만 눌려있어도 조합키 사용
    if(kIsAlphabetScanCode(bDownScanCode) == TRUE)
    {
        if(gs_stKeyboardManager.bShiftDown ^ gs_stKeyboardManager.bCapsLockOn)
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
    }
    // 숫자/기호 는 Shift 가 눌렸을때 조합키 사용
    else if(kIsNumberOrSymbolScanCode(bDownScanCode) == TRUE)
    {
        if(gs_stKeyboardManager.bShiftDown == TRUE)
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
    }
    // 숫자 패드 키라면 NumLock 영향을 받음
    // 0xE0만 제외하면 확장 키 코드와 숫자 패드의 코드가 겹치므로,
    // 확장 키 코드가 수신되지 않았을때만 조합된 코드 사용
    else if((kIsNumberPadScanCode(bDownScanCode) == TRUE) &&
            (gs_stKeyboardManager.bExtendedCodeIn == FALSE))
    {
        if(gs_stKeyboardManager.bNumLockOn == TRUE)
        {
            bUseCombinedKey = TRUE;
        }
        else
        {
            bUseCombinedKey = FALSE;
        }
    }
    return bUseCombinedKey;
}


// 스캔 코드를 ASCII 코드로 변환 
BOOL kConvertScanCodeToASCIICode(BYTE bScanCode, BYTE* pbASCIICode, BOOL* pbFlags)
{
    BOOL bUseCombinedKey;
    
    // 이전에 Pause 키가 수신되었다면, Pause의 남은 스캔 코드를 무시 
    if(gs_stKeyboardManager.iSkipCountForPause > 0)
    {
        gs_stKeyboardManager.iSkipCountForPause--;
        return FALSE;
    }

    // Pause 키 처리 
    if(bScanCode == 0xE1)
    {
        *pbASCIICode = KEY_PAUSE;
        *pbFlags = KEY_FLAGS_DOWN;
        gs_stKeyboardManager.iSkipCountForPause = KEY_SKIPCOUNTFORPAUSE;
        return TRUE;
    }
    // 확장키 처리, 실제 키 값은 0xE0 다음값이므로 플래그만 설정하고 리턴
    else if(bScanCode == 0xE0)
    {
        gs_stKeyboardManager.bExtendedCodeIn = TRUE;
        return FALSE;
    }
    
    // 조합된 키를 반환해야 하는가 판단 
    bUseCombinedKey = kIsUseCombinedCode(bScanCode);

    // 키 값 설정 
    if(bUseCombinedKey == TRUE)
    {
        *pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bCombinedCode;
    }
    else
    {
        *pbASCIICode = gs_vstKeyMappingTable[bScanCode & 0x7F].bNormalCode;
    }

    // 확장 키 유무 설정
    if(gs_stKeyboardManager.bExtendedCodeIn == TRUE)
    {
        *pbFlags = KEY_FLAGS_EXTENDEDKEY;
        gs_stKeyboardManager.bExtendedCodeIn = FALSE;
    }
    else
    {
        *pbFlags = 0;
    }

    // 눌러짐이나 떨어짐 유무 설정
    if((bScanCode & 0x80) == 0)
    {
        *pbFlags |= KEY_FLAGS_DOWN;
    }

    // 조합 키 눌림이나 떨어짐 상태를 갱신 
    UpdateCombinationKeyStatusAndLED(bScanCode);

    return TRUE;
}