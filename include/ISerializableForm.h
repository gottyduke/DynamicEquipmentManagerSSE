#pragma once

#undef SetForm
#undef GetForm

#include "RE/Skyrim.h"
#include "SKSE/Interfaces.h"


enum : UInt32
{
	kInvalid = static_cast<UInt32>(-1)
};


class ISerializableForm
{
public:
	ISerializableForm();
	ISerializableForm(ISerializableForm&) = default;
	ISerializableForm(ISerializableForm&&) = default;
	~ISerializableForm() = default;

	ISerializableForm& operator=(const ISerializableForm&) = default;
	ISerializableForm& operator=(ISerializableForm&&) = default;

	void Clear();
	bool Save(SKSE::SerializationInterface* a_intfc, UInt32 a_type, UInt32 a_version) const;
	bool Save(SKSE::SerializationInterface* a_intfc) const;
	bool Load(SKSE::SerializationInterface* a_intfc);
	void SetForm(UInt32 a_formID);
	[[nodiscard]] RE::TESForm* GetForm() const;
	[[nodiscard]] UInt32 GetFormID() const;

protected:
	UInt32 _formID;
};
