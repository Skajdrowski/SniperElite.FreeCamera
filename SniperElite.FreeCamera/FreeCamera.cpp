#include "core.h"
#include "FreeCamera.h"
#include "se1/Camera.h"
#include "SettingsMgr.h"

Camera* FreeCamera::cam = nullptr;
unsigned int FreeCamera::ms_bEnabled = 0;

uintptr_t FreeCamera::camControl;

uintptr_t FreeCamera::fpsAddr;

uintptr_t FreeCamera::lowerText;
uintptr_t FreeCamera::lowerOffset;
uintptr_t FreeCamera::upperText;
uintptr_t FreeCamera::HUD;

uintptr_t FreeCamera::timeControl;
uintptr_t FreeCamera::timeAddr;

uintptr_t FreeCamera::FoVAddr;

uintptr_t FreeCamera::cullingAddr;

float FreeCamera::timePause;
float FreeCamera::defaultFoV;

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

			if (!fpsAddr)
				fpsAddr = SigScan("C7 ? ? ? ? ? ? ? ? ? D9 ? ? ? ? ? D8 ? ? ? ? ? D9 ? ? ? ? ? D9 ? ? ? ? ? D8 ? ? ? ? ? D9", true, 2);

			if (!lowerText && !upperText && !HUD)
			{
				lowerText = SigScan("D8 ? ? ? ? ? D9 ? ? D9 ? ? ? D8 ? ? ? ? ? D9 ? ? ? ? ? D9 ? ? ? D8", false);
				Read(lowerText + 2, lowerOffset);
				upperText = SigScan("8B ? ? ? 83 ? ? 8D ? ? ? 50 89", false);
				HUD = SigScan("D9 ? ? ? D9 ? ? ? D8 ? ? D9 ? ? D9 ? ? ? D8 ? ? D9 ? ? D9 ? ? ? D8", false);
			}
			Patch(lowerText, {0xD8, 0xC8, 0x90, 0x90, 0x90, 0x90});
			Nop(upperText, 4);
			Nop(HUD, 4);

			if (!timeControl)
			{
				timeControl = SigScan("A3 ? ? ? ? DF ? F6 ? ? 75", false);
				Read(timeControl + 1, timeAddr);
			}
			Nop(timeControl, 5);
			if (timePause != *((float*)timeAddr))
				Read(timeAddr, timePause);

			if (!FoVAddr)
				FoVAddr = SigScan("D8 ? ? ? ? ? A1 ? ? ? ? C3 D8", true, 2);

			if (!defaultFoV)
				Read(FoVAddr, defaultFoV);
			if (FoVFactor != defaultFoV)
				FoVFactor = defaultFoV;

			if (!camControl)
				camControl = SigScan("89 ? 8B ? ? 89 ? ? 8B ? ? 89 ? ? C3 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 56", false);

			Nop(camControl, 2);
			Nop(camControl + 5, 3);
			Nop(camControl + 11, 3);

			if (!cullingAddr)
				cullingAddr = SigScan("C6 ? ? ? ? ? ? 5B E9 ? ? ? ? 5B", true, 2);
			Patch(cullingAddr, 0);

			ms_bEnabled = 2;
		}
		else if (ms_bEnabled == 2)
		{
			const float framerate = *((float*)fpsAddr) / 60.f;
			float speed = SettingsMgr->fFreeCameraSpeed * framerate;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeySlowDown))
				speed /= SettingsMgr->fFreeCameraModifierScale;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeySpeedUp))
				speed *= SettingsMgr->fFreeCameraModifierScale;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyPause) & 0x1)
			{
				if (timePause >= 1.f)
					timePause = 0.f;
				else
					timePause++;
				Patch(timeAddr, timePause);
			}

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyFoVDecrease))
			{
				FoVFactor -= 0.1f * framerate; FoVFactor = clampf(FoVFactor, 1.f, 165.f);
				Patch(FoVAddr, FoVFactor);
			}
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyFoVIncrease))
			{
				FoVFactor += 0.1f * framerate; FoVFactor = clampf(FoVFactor, 1.f, 165.f);
				Patch(FoVAddr, FoVFactor);
			}

			Vector fwd = cam->Rotation.GetForward();
			Vector up = cam->Rotation.GetUp();
			Vector right = cam->Rotation.GetRight();
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyForward))
				cam->Position += fwd * speed;
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyBack))
				cam->Position -= fwd * speed;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyUp))
				cam->Position += up * speed;
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyDown))
				cam->Position -= up * speed;

			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyRight))
				cam->Position += right * speed;
			if (GetAsyncKeyState(SettingsMgr->iFreeCameraKeyLeft))
				cam->Position -= right * speed;
		}
		else if (ms_bEnabled == 3)
		{
			Patch(lowerText, {0xD8, 0x0D}); Patch(lowerText + 2, lowerOffset);
			Patch(upperText, {0x8B, 0x5C, 0x24, 0x1C});
			Patch(HUD, {0xD9, 0x5C, 0x24, 0x0C});

			Patch(timeControl, {0xA3}); Patch(timeControl + 1, timeAddr);

			Patch(FoVAddr, defaultFoV);

			Patch(camControl, {0x89, 0x11});
			Patch(camControl + 5, {0x89, 0x51, 0x04});
			Patch(camControl + 11, {0x89, 0x41, 0x08});

			if (*((uint32_t*)cullingAddr) == 0)
				Patch(cullingAddr, 1);

			ms_bEnabled = 0;
		}

		Sleep(1);
	}
}
