#include "IniReader.h"
#include "SettingsMgr.h"
#include <Windows.h>

eSettingsManager* SettingsMgr = new eSettingsManager;

eSettingsManager::eSettingsManager()
{
	CIniReader ini("");

	iFreeCameraEnableKey = ini.ReadInteger("Settings", "iFreeCameraEnableKey", 0);
	iFreeCameraKeyForward = ini.ReadInteger("Settings", "iFreeCameraKeyForward", 0);
	iFreeCameraKeyBack = ini.ReadInteger("Settings", "iFreeCameraKeyBack", 0);
	iFreeCameraKeyLeft = ini.ReadInteger("Settings", "iFreeCameraKeyLeft", 0);
	iFreeCameraKeyRight = ini.ReadInteger("Settings", "iFreeCameraKeyRight", 0);
	iFreeCameraKeyUp = ini.ReadInteger("Settings", "iFreeCameraKeyUp", 0);
	iFreeCameraKeyDown = ini.ReadInteger("Settings", "iFreeCameraKeyDown", 0);
	iFreeCameraKeySlowDown = ini.ReadInteger("Settings", "iFreeCameraKeySlowDown", 0);
	iFreeCameraKeySpeedUp = ini.ReadInteger("Settings", "iFreeCameraKeySpeedUp", 0);
	iFreeCameraKeyFoVIncrease = ini.ReadInteger("Settings", "iFreeCameraKeyFoVIncrease", 0);
	iFreeCameraKeyFoVDecrease = ini.ReadInteger("Settings", "iFreeCameraKeyFoVDecrease", 0);
	iFreeCameraKeyPause = ini.ReadInteger("Settings", "iFreeCameraKeyPause", 0);
	fFreeCameraSpeed = ini.ReadFloat("Settings", "fFreeCameraSpeed", 0.05f);
	fFreeCameraModifierScale = ini.ReadFloat("Settings", "fFreeCameraModifierScale", 4.0f);
}

void eSettingsManager::SaveSettings()
{
}

void eSettingsManager::ResetKeys()
{
	iFreeCameraEnableKey = VK_F1;
	iFreeCameraKeyForward = 104;
	iFreeCameraKeyBack = 98;
	iFreeCameraKeyLeft = 102;
	iFreeCameraKeyRight = 100;
	iFreeCameraKeyUp = 103;
	iFreeCameraKeyDown = 97;
	iFreeCameraKeySlowDown = 101;
	iFreeCameraKeySpeedUp = 96;
	iFreeCameraKeyFoVIncrease = 107;
	iFreeCameraKeyFoVDecrease = 109;
	iFreeCameraKeyPause = VK_PAUSE;

	fFreeCameraSpeed = 0.05f;
	fFreeCameraModifierScale = 4.0f;
}
