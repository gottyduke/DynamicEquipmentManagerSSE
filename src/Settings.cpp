#include "settings.h"


bool Settings::LoadSettings(bool a_dumpParse)
{
	auto [log, success] = Json2Settings::load_settings(FILE_NAME, a_dumpParse);
	if (!log.empty()) {
		_ERROR("%s", log.c_str());
	}
	return success;
}


decltype(Settings::manageAmmo) Settings::manageAmmo("manageAmmo", true);
decltype(Settings::manageHelmet) Settings::manageHelmet("manageHelmet", true);
decltype(Settings::manageShield) Settings::manageShield("manageShield", true);
