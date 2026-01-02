#pragma once

struct Camera;

class FreeCamera {
public:
	static void Init();
	static void Thread();
private:
	static unsigned int ms_bEnabled;
	static constexpr float clampf(float v, float lo, float hi);
	static float defaultFoV;
	static float timePause;
	static Camera* cam;
};