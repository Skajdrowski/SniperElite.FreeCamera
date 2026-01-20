#include "Camera.h"
#include "..\FreeCamera.h"
#include "..\core.h"
Camera* GetCamera()
{
	return *(Camera**)(SigScan("8B ? ? ? ? ? C6 ? ? ? ? ? ? 8B ? 89 ? 8B ? ? 89", true, 2));
}
