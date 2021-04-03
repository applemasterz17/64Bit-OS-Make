#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

#include "Types.h"

////////////////////////////////////////////////////////
// 함수 
////////////////////////////////////////////////////////
BYTE kInPortByte(WORD wPort);               // 출력버퍼(OutputBuffer) 를 읽어올때 쓴다 
void kOutPortByte(WORD wPort, BYTE bData);  // 입력버퍼(InputBuffer) 에 데이터를 쓸때 쓴다

#endif /*__ASSEMBLYUTILITY_H__*/