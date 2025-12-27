#pragma once

struct Camera;

class FreeCamera {
public:
	static void Init();
	static void Thread();
private:
	static unsigned int ms_bEnabled;
	static Camera* cam;
};