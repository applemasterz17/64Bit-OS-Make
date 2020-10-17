#include "Page.h"

void kInitializePageTables(void)
{
    PML4TENTRY* pstPML4Entry;
    PDPTENTRY* pstPDPTEntry;
    PDENTRY* pstPDEntry;
    DWORD dwMappingAddress;
    int i;

    // Create PML4 Table 
    // 첫 번째 엔트리 외에 나머지는 0으로 초기화 
    pstPML4Entry = (PML4TENTRY *) 0x100000;
    kSetPageEntryData(&(pstPML4Entry[0]), 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0);
    for(i=1; i<PAGE_MAXENTRYCOUNT; i++)
    {
        kSetPageEntryData(&(pstPML4Entry[i]), 0, 0, 0, 0);
    }

    // Create Page Directory Pointer Table 
    // 하나의 PDPT로 512GB 까지 매핑 가능하므로 하나로 충분
    // 64개의 엔트리를 설정하여 64GB까지 매핑함 
    pstPDPTEntry = (PDPTENTRY *) 0x101000;
    for(i=0; i<64; i++)
    {
        kSetPageEntryData(&(pstPDPTEntry[i]), 0, 0x102000 + (i * PAGE_TABLESIZE), PAGE_FLAGS_DEFAULT, 0);
    }
    for(i=64; i<PAGE_MAXENTRYCOUNT; i++)
    {
        kSetPageEntryData(&(pstPDPTEntry[i]), 0, 0, 0, 0);
    }

    // Create Page Directory Table
    // 하나의 페이지 디렉터리가 1GB까지 매핑 가능
    // 여유있게 64개의 페이지 디렉터리를 생성하여 총 64GB까지 지원
    pstPDEntry = (PDENTRY *) 0x102000;
    dwMappingAddress = 0;
    for(i=0; i<PAGE_MAXENTRYCOUNT * 64; i++)
    {
        kSetPageEntryData(&(pstPDEntry[i]), (i*(PAGE_DEFAULTSIZE >> 20)) >> 12, dwMappingAddress, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0);
        dwMappingAddress += PAGE_DEFAULTSIZE;
    }
}

void kSetPageEntryData(PTENTRY* pstEntry, DWORD dwUpperBaseAddress, 
        DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags)
{
    pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
    pstEntry->dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF) | dwUpperFlags;
}

