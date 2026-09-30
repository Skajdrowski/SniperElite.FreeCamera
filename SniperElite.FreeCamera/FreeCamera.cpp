#include "core.h"
#include "FreeCamera.h"
#include "se1/Camera.h"
#include "SettingsMgr.h"

Camera* FreeCamera::cam = nullptr;
FreeCamera::State FreeCamera::ms_bEnabled = Disabled;

uintptr_t FreeCamera::camControl = 0;
uintptr_t FreeCamera::playerAimUpdateControl = 0;
size_t FreeCamera::playerAimUpdateCallOffset = 0;
uintptr_t FreeCamera::playerRotationControl = 0;
size_t FreeCamera::playerRotationCallOffset = 0;
uintptr_t FreeCamera::playerMovementRotationControl = 0;

float* FreeCamera::fpsAddr = nullptr;

uintptr_t FreeCamera::lowerText = 0;
uintptr_t FreeCamera::upperText = 0;
uintptr_t FreeCamera::HUD = 0;

uintptr_t FreeCamera::timeControl = 0;
float* FreeCamera::timeAddr = nullptr;

float* FreeCamera::FoVAddr = nullptr;

unsigned int* FreeCamera::cullingAddr = nullptr;

float FreeCamera::timePause = 0.0f;
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
			ms_bEnabled = (State)(ms_bEnabled + 1);

		if (ms_bEnabled == Enabling)
		{
			if (!cam)
				cam = GetCamera();

			if (!fpsAddr)
				fpsAddr = reinterpret_cast<float*>(SigScan("C7 ? ? ? ? ? ? ? ? ? D9 ? ? ? ? ? D8 ? ? ? ? ? D9 ? ? ? ? ? D9 ? ? ? ? ? D8 ? ? ? ? ? D9", true, 2));

			if (!lowerText)
				lowerText = SigScan("56 8B ? E8 ? ? ? ? 84 ? 74 ? A0 ? ? ? ? 84 ? 0F", false);
			if (!upperText)
				upperText = SigScan("83 ? ? 57 8B ? ? ? 85 ? 0F ? ? ? ? ? D9", false);
			if (!HUD)
				HUD = SigScan("8B ? ? 8B ? FF ? ? EB ? 8B ? ? 8B ? FF ? ? A0", false, 5);

			Patch(lowerText, {0xC2, 0x04, 0x00});
			Patch(upperText, {0xC2, 0x0C, 0x00});

			Nop(HUD, 3);
			Nop(HUD + 10, 3);

			if (!timeControl)
			{
				timeControl = SigScan("A3 ? ? ? ? DF ? F6 ? ? 75", false);
				Read(timeControl + 1, timeAddr);
			}
			Nop(timeControl, 5);
			if (timePause != *timeAddr)
				Read(timeAddr, timePause);

			if (!FoVAddr)
				FoVAddr = reinterpret_cast<float*>(SigScan("D8 ? ? ? ? ? A1 ? ? ? ? C3 D8", true, 2));

			if (!defaultFoV)
				Read(FoVAddr, defaultFoV);
			if (FoVFactor != defaultFoV)
				FoVFactor = defaultFoV;

			if (!camControl)
				camControl = SigScan("89 ? 8B ? ? 89 ? ? 8B ? ? 89 ? ? C3 ? ? ? ? ? ? ? ? ? ? ? ? ? ? 56", false);
			Nop(camControl, 2);
			Nop(camControl + 5, 3);
			Nop(camControl + 11, 3);

			if (!playerAimUpdateControl)
			{
				playerAimUpdateControl = SigScan("57 8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B 4E", false);
				Read(playerAimUpdateControl + 4, playerAimUpdateCallOffset);
			}
			Nop(playerAimUpdateControl, 8);

			if (!playerRotationControl)
			{
				playerRotationControl = SigScan("8B 4E 64 E8 ? ? ? ? 50 8B CF E8 ? ? ? ? 8B 56 64 85 D2 C6 86 04 01 00 00 01", false, 8);
				Read(playerRotationControl + 4, playerRotationCallOffset);
			}
			Nop(playerRotationControl, 8);

			if (!playerMovementRotationControl)
				playerMovementRotationControl = SigScan("8B ? ? ? ? ? 8B ? C1 ? ? 81 ? ? ? ? ? 52", false, 6);
			Patch(playerMovementRotationControl, {0x33, 0xD2});

			if (!cullingAddr)
				cullingAddr = reinterpret_cast<unsigned int*>(SigScan("C6 ? ? ? ? ? ? 5B E9 ? ? ? ? 5B", true, 2));
			Patch(cullingAddr, 0);

			ms_bEnabled = Enabled;
		}
		else if (ms_bEnabled == Enabled)
		{
			const float framerate = *fpsAddr / 60.f;
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
		else if (ms_bEnabled == Disabling)
		{
			Patch(lowerText, {0x56, 0x8B, 0xF1});
			Patch(upperText, {0x83, 0xEC, 0x20});
			Patch(HUD, {0xFF, 0x50, 0x0C});
			Patch(HUD + 10, {0xFF, 0x52, 0x0C});

			Patch(timeControl, {0xA3}); Patch(timeControl + 1, timeAddr);

			Patch(FoVAddr, defaultFoV);

			Patch(camControl, {0x89, 0x11});
			Patch(camControl + 5, {0x89, 0x51, 0x04});
			Patch(camControl + 11, {0x89, 0x41, 0x08});

			Patch(playerAimUpdateControl, {0x57, 0x8B, 0xCE, 0xE8,
				static_cast<uint8_t>(playerAimUpdateCallOffset),
				static_cast<uint8_t>(playerAimUpdateCallOffset >> 8),
				static_cast<uint8_t>(playerAimUpdateCallOffset >> 16),
				static_cast<uint8_t>(playerAimUpdateCallOffset >> 24)});
			Patch(playerRotationControl, {0x50, 0x8B, 0xCF, 0xE8,
				static_cast<uint8_t>(playerRotationCallOffset),
				static_cast<uint8_t>(playerRotationCallOffset >> 8),
				static_cast<uint8_t>(playerRotationCallOffset >> 16),
				static_cast<uint8_t>(playerRotationCallOffset >> 24)});
			Patch(playerMovementRotationControl, {0x8B, 0xD0});

			if (*cullingAddr == 0)
				Patch(cullingAddr, 1);

			ms_bEnabled = Disabled;
		}

		Sleep(1);
	}
}
