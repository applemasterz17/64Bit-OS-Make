#!/bin/sh
# 64비트
# -L : BIOS 경로
# -m : 메모리 [64 메가바이트]
# -fda : 플로피 디스크 부팅
# -M : ???

qemu-system-x86_64 -L . -m 64 -fda ./Disk.img -M pc -s -S
