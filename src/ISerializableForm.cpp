#include "ISerializableForm.h"

#include "RE/Skyrim.h"
#include "SKSE/Interfaces.h"


ISerializableForm::ISerializableForm() :
	_formID(kInvalid)
{}


void ISerializableForm::Clear()
{
	_formID = kInvalid;
}


bool ISerializableForm::Save(SKSE::SerializationInterface* a_intfc, const UInt32 a_type, const UInt32 a_version) const
{
	if (!a_intfc->OpenRecord(a_type, a_version)) {
		_ERROR("Failed to open serialization record!\n");
		return false;
	}
	return Save(a_intfc);
}


bool ISerializableForm::Save(SKSE::SerializationInterface* a_intfc) const
{
	a_intfc->WriteRecordData(&_formID, sizeof(_formID));

	return true;
}


bool ISerializableForm::Load(SKSE::SerializationInterface* a_intfc)
{
	a_intfc->ReadRecordData(&_formID, sizeof(_formID));
	if (!a_intfc->ResolveFormID(_formID, _formID)) {
		_ERROR("Failed to resolve formID");
		_formID = kInvalid;
		return false;
	}

	return true;
}


void ISerializableForm::SetForm(const UInt32 a_formID)
{
	_formID = a_formID;
}


RE::TESForm* ISerializableForm::GetForm() const
{
	return _formID == kInvalid ? nullptr : RE::TESForm::LookupByID(_formID);
}


UInt32 ISerializableForm::GetFormID() const
{
	return _formID;
}
