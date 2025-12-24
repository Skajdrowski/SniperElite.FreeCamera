#include "core.h"
#include "FreeCamera.h"
#include "se1/Camera.h"
#include "SettingsMgr.h"

bool FreeCamera::ms_bEnabled = false;

void FreeCamera::Init()
{
	CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Thread), nullptr, 0, nullptr);
}

void FreeCamera::Thread()
{
	while (true)
	{
		Camera* cam = GetCamera();
		if (GetAsyncKeyState(SettingsMgr->iFreeCameraEnableKey) & 0x1)
			ms_bEnabled ^= 1;

		if (ms_bEnabled)
		{
			Nop(_addr(0x15303), 2);
			Nop(_addr(0x15308), 3);
			Nop(_addr(0x1530E), 3);

			if (cam)
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
					cam->Position += right * speed * -1;
				if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyLeft))
					cam->Position += right * speed * 1;
			}
		}
		else
		{
			Patch(_addr(0x15303), {0x89, 0x11});
			Patch(_addr(0x15308), {0x89, 0x51, 0x04});
			Patch(_addr(0x1530E), {0x89, 0x41, 0x08});
		}

		Sleep(1);
	}
}
