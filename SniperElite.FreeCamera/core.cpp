#include "core.h"

int GetEntryPoint()
{
	static int addr = reinterpret_cast<__int64>(GetModuleHandle(nullptr));
	return addr;
}

int _addr(int addr)
{
	return GetEntryPoint() + addr;
}

uintptr_t SigScan(const char* pattern, bool read, uint32_t pad)
{
	hook::pattern Pattern(pattern);
	uintptr_t OpcodeAddr = Pattern.count_hint(1).get(0).get_uintptr(pad);
	if (read)
		Read(OpcodeAddr, OpcodeAddr);

	return OpcodeAddr;
}
