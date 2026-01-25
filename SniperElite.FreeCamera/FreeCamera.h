#pragma once

struct Camera;

class FreeCamera {
public:
	static void Init();
	static void Thread();
private:
	static Camera* cam;

	enum State {
		Disabled,
		Enabling,
		Enabled,
		Disabling
	};
	static State ms_bEnabled;

	static uintptr_t camControl;

	static float* fpsAddr;

	static uintptr_t lowerText;
	static uintptr_t lowerOffset;
	static uintptr_t upperText;
	static uintptr_t HUD;
	static uintptr_t AC;

	static uintptr_t timeControl;
	static float* timeAddr;

	static float* FoVAddr;

	static unsigned int* cullingAddr;

	static float timePause;
	static float defaultFoV;

	static constexpr float clampf(float v, float lo, float hi);
};