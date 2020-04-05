#pragma once

#include <string>
#include <utility>

#include "RE/Skyrim.h"


namespace
{
	enum : UInt32
	{
		kWeapTypeBoundArrow = 0x10D501,
		kWerewolfBeastRace = 0x0CDD84,
		kDLC1VampireBeastRace = 0x00283A
	};
}


template <typename T>
class Form
{
public:
	enum : UInt32 { kInvalid = static_cast<UInt32>(-1) };


	Form(const UInt32 a_rawFormID, std::string a_pluginName) :
		_rawFormID(a_rawFormID),
		_loadedFormID(kInvalid),
		_pluginName(std::move(a_pluginName))
	{}


	// can't use explicit here
	operator T*()
	{
		if (_rawFormID == kInvalid) {
			return nullptr;
		}

		if (_loadedFormID == kInvalid) {
			auto dataHandler = RE::TESDataHandler::GetSingleton();
			const auto modInfo = dataHandler->LookupLoadedModByName(_pluginName.c_str());
			if (modInfo) {
				_loadedFormID = _rawFormID + (modInfo->compileIndex << (3 * 8));
			} else {
				_rawFormID = kInvalid;
				return nullptr;
			}
		}

		return RE::TESForm::LookupByID<T>(_loadedFormID);
	}

private:
	UInt32 _rawFormID;
	UInt32 _loadedFormID;
	std::string _pluginName;
};


extern Form<RE::BGSKeyword> WeapTypeBoundArrow;
extern Form<RE::TESRace> WerewolfBeastRace;
extern Form<RE::TESRace> DLC1VampireBeastRace;
