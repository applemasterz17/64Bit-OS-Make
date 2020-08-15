#include <stdio.h>

int main()
{
	int totalSectorNum = 1024;
	int headNum = 0;	// 0 or 1 
	int sectorNum = 2;	// 1 ~ 18
	int trackNum = 0;	// 0 ~ 79 
	char *memAddr = 0x10000;

	/*
		반복하면서,
		totalSectorNum 이 0 이 될때까지, 
		totalSectorNum 한개씩 감소 = 1024 섹터만큼 복사할꺼임 
		memAddr 로 복사하는 함수 실행 
		memAddr 값 0x200 증가 = 섹터 하나당 512b(=0x200)
		섹터 -> 헤드 -> 트랙 순서대로 증가
		섹터가 19 미만이면 위 과정 다시 실행 
		섹터가 19면 헤드 ^ 0x01 연산 해버리고 섹터 1로 변경 
		헤드가 0이 아니라면 위 과정 반복 
		헤드가 0이면 트랙 +1 
	*/
	
	while(1)
	{
		if(totalSectorNum == 0)
		{
			break;
		}

		totalSectorNum = totalSectorNum - 1;

		copyFunc(sectorNum, headNum, trackNum, memAddr);

		memAddr = memAddr + 0x200;
		sectorNum = sectorNum + 1

		if(sectorNum < 19)
		{
			continue;
		}

		headNum = headNum ^ 0x01;
		sectorNum = 1;

		if(headNum =! 0)
		{
			continue;
		}

		trackNum = trackNum + 1

	}
	
	return 0;
}