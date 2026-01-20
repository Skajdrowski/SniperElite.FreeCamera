#pragma once

struct Camera;

class FreeCamera {
public:
	static void Init();
	static void Thread();
private:
	static Camera* cam;
	static unsigned int ms_bEnabled;

	static uintptr_t camControl;

	static uintptr_t fpsAddr;

	static uintptr_t lowerText;
	static uintptr_t lowerOffset;
	static uintptr_t upperText;
	static uintptr_t HUD;

	static uintptr_t timeControl;
	static uintptr_t timeAddr;

	static uintptr_t FoVAddr;

	static uintptr_t cullingAddr;

	static float timePause;
	static float defaultFoV;

	static constexpr float clampf(float v, float lo, float hi);
};