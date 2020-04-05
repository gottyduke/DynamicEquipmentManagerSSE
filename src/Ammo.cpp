#include "Ammo.h"

#include "Forms.h"  // WeapTypeBoundArrow
#include "PlayerUtil.h"  // PlayerIsBeastRace

#include "RE/Skyrim.h"


namespace Ammo
{
	auto Ammo::GetSingleton()
	-> Ammo*
	{
		static Ammo singleton;
		return std::addressof(singleton);
	}


	auto Ammo::GetForm() const
	-> RE::TESAmmo*
	{
		return static_cast<RE::TESAmmo*>(ISerializableForm::GetForm());
	}


	void DelayedAmmoTaskDelegate::Run()
	{
		if (g_equippedAmmoFormID != kInvalid) {
			const auto ammo = RE::TESForm::LookupByID<RE::TESAmmo>(g_equippedAmmoFormID);
			if (!ammo->HasKeyword(WeapTypeBoundArrow)) {
				if (g_equippedWeaponFormID == kInvalid) {
					Ammo::GetSingleton()->SetForm(g_equippedAmmoFormID);
				} else {
					// Ammo was force equipped
					Visitor visitor;
					VisitPlayerInventoryChanges(&visitor);
				}
			}
			g_equippedAmmoFormID = kInvalid;
		}
	}


	void DelayedAmmoTaskDelegate::Dispose()
	{
		delete this;
	}


	bool DelayedAmmoTaskDelegate::Visitor::Accept(RE::InventoryEntryData* a_entry, const SInt32 a_count)
	{
		if (a_entry->GetObject()->GetFormID() == g_equippedAmmoFormID && a_entry->extraLists) {
			for (auto& xList : *a_entry->extraLists) {
				if (xList->HasType(RE::ExtraDataType::kWorn) || xList->HasType(RE::ExtraDataType::kWornLeft)) {
					auto equipManager = RE::ActorEquipManager::GetSingleton();
					const auto player = RE::PlayerCharacter::GetSingleton();
					equipManager->UnequipObject(player, a_entry->GetObject(), xList, a_count, nullptr, true, false);

					auto ui = RE::UI::GetSingleton();
					const auto uiStr = RE::InterfaceStrings::GetSingleton();
					auto invMenu = ui->GetMenu<RE::InventoryMenu>(uiStr->inventoryMenu);
					if (invMenu && invMenu->InventoryItemMenu()) {
						invMenu->itemList->Update(player);
					}

					return false;
				}
			}
		}
		return true;
	}


	void DelayedWeaponTaskDelegate::Run()
	{
		if (g_equippedWeaponFormID != kInvalid) {
			const auto weap = RE::TESForm::LookupByID<RE::TESObjectWEAP>(g_equippedWeaponFormID);

			if ((!weap->IsBow() && !weap->IsCrossbow()) || weap->IsBound()) {
				g_equippedWeaponFormID = kInvalid;
				return;
			}

			Visitor visitor;
			VisitPlayerInventoryChanges(&visitor);

			const auto ammo = Ammo::GetSingleton()->GetForm();
			if (ammo) {
				auto equipManager = RE::ActorEquipManager::GetSingleton();
				const auto player = RE::PlayerCharacter::GetSingleton();
				equipManager->EquipObject(player, ammo, nullptr, visitor.Count(), nullptr, true, false, false);

				auto ui = RE::UI::GetSingleton();
				const auto uiStr = RE::InterfaceStrings::GetSingleton();
				auto invMenu = ui->GetMenu<RE::InventoryMenu>(uiStr->inventoryMenu);
				if (invMenu && invMenu->InventoryItemMenu()) {
					invMenu->itemList->Update(player);
				}
			}

			g_equippedWeaponFormID = kInvalid;
		}
	}


	void DelayedWeaponTaskDelegate::Dispose()
	{
		delete this;
	}


	DelayedWeaponTaskDelegate::Visitor::Visitor() :
		_count(0)
	{}


	bool DelayedWeaponTaskDelegate::Visitor::Accept(RE::InventoryEntryData* a_entry, const SInt32 a_count)
	{
		if (a_entry->GetObject()->IsAmmo()) {
			if (a_entry->GetObject()->GetFormID() == Ammo::GetSingleton()->GetFormID()) {
				_count = a_count;
				return false;
			}
		}
		return true;
	}


	auto DelayedWeaponTaskDelegate::Visitor::Count() const
	-> SInt32
	{
		return _count;
	}


	bool TESEquipEventHandler::Visitor::Accept(RE::InventoryEntryData* a_entry, const SInt32 a_count)
	{
		if (a_entry->GetObject()->GetFormID() == Ammo::GetSingleton()->GetFormID() && a_entry->extraLists) {
			auto equipManager = RE::ActorEquipManager::GetSingleton();
			const auto player = RE::PlayerCharacter::GetSingleton();
			const auto xList = a_entry->extraLists->empty() ? nullptr : a_entry->extraLists->front();
			equipManager->UnequipObject(player, a_entry->GetObject(), xList, a_count, nullptr, true, false);
			return false;
		}
		return true;
	}


	auto TESEquipEventHandler::GetSingleton()
	-> TESEquipEventHandler*
	{
		static TESEquipEventHandler singleton;
		return std::addressof(singleton);
	}


	auto TESEquipEventHandler::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource)
	-> EventResult
	{
		if (!a_event || !a_event->actor || !a_event->actor->IsPlayerRef() || PlayerIsBeastRace()) {
			return EventResult::kContinue;
		}

		const auto form = RE::TESForm::LookupByID(a_event->baseObject);
		if (!form) {
			return EventResult::kContinue;
		}

		const auto task = SKSE::GetTaskInterface();
		switch (form->GetFormType()) {
		case RE::FormType::Weapon:
			if (a_event->equipped) {
				g_equippedWeaponFormID = form->GetFormID();
				task->AddTask(new DelayedWeaponTaskDelegate());
			} else {
				Visitor visitor;
				VisitPlayerInventoryChanges(&visitor);
			}
			break;
		case RE::FormType::Ammo:
			if (a_event->equipped) {
				g_equippedAmmoFormID = form->GetFormID();
				task->AddTask(new DelayedAmmoTaskDelegate());
			}
			break;
		default: ;
		}
		return EventResult::kContinue;
	}
}
