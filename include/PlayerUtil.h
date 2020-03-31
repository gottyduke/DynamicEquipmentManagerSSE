#pragma once

#include "RE/Skyrim.h"


namespace
{
	using FormID = UInt32;
	using Count = SInt32;
}


class InventoryChangesVisitor
{
public:
	InventoryChangesVisitor() = default;
	virtual ~InventoryChangesVisitor() = default;

	virtual bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) = 0;
};


void VisitPlayerInventoryChanges(InventoryChangesVisitor* a_visitor);
bool SinkAnimationGraphEventHandler(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink);
bool PlayerIsBeastRace();
