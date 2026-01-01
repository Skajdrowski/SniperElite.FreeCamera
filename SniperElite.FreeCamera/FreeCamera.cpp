#include "core.h"
#include "FreeCamera.h"
#include "se1/Camera.h"
#include "SettingsMgr.h"

Camera* FreeCamera::cam = nullptr;
unsigned int FreeCamera::ms_bEnabled = 0;
float FreeCamera::defaultFoV = 0.0f;

constexpr float FreeCamera::clampf(float v, float lo, float hi)
{
	return v < lo ? lo : v > hi ? hi : v;
}

void FreeCamera::Init()
{
	CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Thread), nullptr, 0, nullptr);
}

void FreeCamera::Thread()
{
	while (true)
	{
		static float FoVFactor;

		if (GetAsyncKeyState(SettingsMgr->iFreeCameraEnableKey) & 0x1)
			ms_bEnabled += 1;

		if (ms_bEnabled == 1)
		{
			if (!cam)
				cam = GetCamera();

			Nop(_addr(0x80FA), 4);
			Nop(_addr(0x102186), 4);

			if (!defaultFoV)
				defaultFoV = *((float*)_addr(0x2FFF08));
			if (FoVFactor != defaultFoV)
				FoVFactor = defaultFoV;

			Nop(_addr(0x15303), 2);
			Nop(_addr(0x15308), 3);
			Nop(_addr(0x1530E), 3);
			ms_bEnabled = 2;
		}
		else if (ms_bEnabled == 2)
		{
			const float framerate = *((float*)_addr(0x368390)) / 60.f;
			float speed = SettingsMgr->fFreeCameraSpeed * framerate;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeySlowDown))
				speed /= SettingsMgr->fFreeCameraModifierScale;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeySpeedUp))
				speed *= SettingsMgr->fFreeCameraModifierScale;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyFoVDecrease))
			{
				FoVFactor -= 0.1f * framerate; FoVFactor = clampf(FoVFactor, 1.f, 165.f);
				Patch(_addr(0x2FFF08), FoVFactor);
			}
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyFoVIncrease))
			{
				FoVFactor += 0.1f * framerate; FoVFactor = clampf(FoVFactor, 1.f, 165.f);
				Patch(_addr(0x2FFF08), FoVFactor);
			}

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

			Patch(_addr(0x2FFF08), defaultFoV);

			Patch(_addr(0x15303), {0x89, 0x11});
			Patch(_addr(0x15308), {0x89, 0x51, 0x04});
			Patch(_addr(0x1530E), {0x89, 0x41, 0x08});
			ms_bEnabled = 0;
		}

		Sleep(1);
	}
}
