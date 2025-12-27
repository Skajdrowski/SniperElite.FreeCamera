#include "core.h"
#include "FreeCamera.h"
#include "se1/Camera.h"
#include "SettingsMgr.h"

Camera* FreeCamera::cam = nullptr;
unsigned int FreeCamera::ms_bEnabled = 0;

void FreeCamera::Init()
{
	CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Thread), nullptr, 0, nullptr);
}

void FreeCamera::Thread()
{
	while (true)
	{
		if (GetAsyncKeyState(SettingsMgr->iFreeCameraEnableKey) & 0x1)
			ms_bEnabled += 1;

		if (ms_bEnabled == 1)
		{
			if (!cam)
				cam = GetCamera();

			Nop(_addr(0x80FA), 4);
			Nop(_addr(0x102186), 4);

			Nop(_addr(0x15303), 2);
			Nop(_addr(0x15308), 3);
			Nop(_addr(0x1530E), 3);
			ms_bEnabled = 2;
		}
		else if (ms_bEnabled == 2)
		{
			float framerate = *((float*)_addr(0x368390));
			float speed = SettingsMgr->fFreeCameraSpeed * (framerate / 60.f);

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeySlowDown))
				speed /= SettingsMgr->fFreeCameraModifierScale;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeySpeedUp))
				speed *= SettingsMgr->fFreeCameraModifierScale;

			Vector fwd = cam->Rotation.GetForward();
			Vector up = cam->Rotation.GetUp();
			Vector right = cam->Rotation.GetRight();
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyForward))
				cam->Position += fwd * speed * 1;
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyBack))
				cam->Position += fwd * speed * -1;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyUp))
				cam->Position += up * speed * 1;
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyDown))
				cam->Position += up * speed * -1;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyRight))
				cam->Position += right * speed * 1;
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyLeft))
				cam->Position += right * speed * -1;
		}
		else if (ms_bEnabled == 3)
		{
			Patch(_addr(0x80FA), {0x66, 0x89, 0x0C, 0x28});
			Patch(_addr(0x102186), {0xD9, 0x5C, 0x24, 0x0C});

			Patch(_addr(0x15303), {0x89, 0x11});
			Patch(_addr(0x15308), {0x89, 0x51, 0x04});
			Patch(_addr(0x1530E), {0x89, 0x41, 0x08});
			ms_bEnabled = 0;
		}

		Sleep(1);
	}
}
