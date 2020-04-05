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

		[[nodiscard]] RE::TESAmmo* GetForm() const;

		Ammo(const Ammo&) = delete;
		Ammo(Ammo&&) = delete;

		Ammo& operator=(const Ammo&) = delete;
		Ammo& operator=(Ammo&&) = delete;

	protected:
		Ammo() = default;
		~Ammo() = default;
	};


	class DelayedWeaponTaskDelegate final : public TaskDelegate
	{
	public:
		class Visitor final : public InventoryChangesVisitor
		{
		public:
			Visitor();

			bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
			[[nodiscard]] SInt32 Count() const;

		private:
			SInt32 _count;
		};


		void Run() override;
		void Dispose() override;
	};


	class DelayedAmmoTaskDelegate : public TaskDelegate
	{
	public:
		class Visitor final : public InventoryChangesVisitor
		{
		public:
			bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		void Run() override;
		void Dispose() override;
	};


	class TESEquipEventHandler final : public RE::BSTEventSink<RE::TESEquipEvent>
	{
	public:
		using EventResult = RE::BSEventNotifyControl;


		class Visitor final : public InventoryChangesVisitor
		{
		public:
			bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		static TESEquipEventHandler* GetSingleton();
		EventResult ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override;

		TESEquipEventHandler(const TESEquipEventHandler&) = delete;
		TESEquipEventHandler(TESEquipEventHandler&&) = delete;

		TESEquipEventHandler& operator=(const TESEquipEventHandler&) = delete;
		TESEquipEventHandler& operator=(TESEquipEventHandler&&) = delete;

	protected:
		TESEquipEventHandler() = default;
		virtual ~TESEquipEventHandler() = default;
	};


	namespace
	{
		UInt32 g_equippedAmmoFormID = kInvalid;
		UInt32 g_equippedWeaponFormID = kInvalid;
	}
}
