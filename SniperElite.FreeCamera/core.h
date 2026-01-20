#pragma once
#include "utils/MemoryMgr.h"
#include "utils/Patterns.h"

using namespace Memory::VP;


int GetEntryPoint();
int _addr(int addr);
uintptr_t SigScan(const char* pattern, bool read = true, uint32_t pad = 0);