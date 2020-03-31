#pragma once

#include "skse64/gamethreads.h"

#include "ISerializableForm.h"  // ISerializableForm, kInvalid
#include "PlayerUtil.h"  // InventoryChangesVisitor

#include "RE/Skyrim.h"
#include "SKSE/API.h"

namespace Ammo
{
	class Ammo : public ISerializableForm
	{
	public:
		static Ammo* GetSingleton();

		RE::TESAmmo* GetForm();

	protected:
		Ammo() = default;
		Ammo(const Ammo&) = delete;
		Ammo(Ammo&&) = delete;
		~Ammo() = default;

		Ammo& operator=(const Ammo&) = delete;
		Ammo& operator=(Ammo&&) = delete;
	};


	class DelayedWeaponTaskDelegate : public TaskDelegate
	{
	public:
		class Visitor : public InventoryChangesVisitor
		{
		public:
			Visitor();
			virtual ~Visitor() = default;

			virtual bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
			SInt32 Count() const;

		private:
			SInt32 _count;
		};


		virtual void Run() override;
		virtual void Dispose() override;
	};


	class DelayedAmmoTaskDelegate : public TaskDelegate
	{
	public:
		class Visitor : public InventoryChangesVisitor
		{
		public:
			virtual bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		virtual void Run() override;
		virtual void Dispose() override;
	};


	class TESEquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent>
	{
	public:
		using EventResult = RE::BSEventNotifyControl;


		class Visitor : public InventoryChangesVisitor
		{
		public:
			virtual bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		static TESEquipEventHandler* GetSingleton();
		virtual EventResult ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override;

	protected:
		TESEquipEventHandler() = default;
		TESEquipEventHandler(const TESEquipEventHandler&) = delete;
		TESEquipEventHandler(TESEquipEventHandler&&) = delete;
		virtual ~TESEquipEventHandler() = default;

		TESEquipEventHandler& operator=(const TESEquipEventHandler&) = delete;
		TESEquipEventHandler& operator=(TESEquipEventHandler&&) = delete;
	};


	namespace
	{
		UInt32 g_equippedAmmoFormID = kInvalid;
		UInt32 g_equippedWeaponFormID = kInvalid;
	}
}
