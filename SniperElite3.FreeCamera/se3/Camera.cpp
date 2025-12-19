#include "Camera.h"
#include "..\FreeCamera.h"
#include "..\core.h"
Camera* GetCamera()
{
	return *(Camera**)(_addr(0x356E44));
}
