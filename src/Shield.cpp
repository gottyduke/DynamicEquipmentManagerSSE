#include "Shield.h"

#include <type_traits>  // typeid

#include "Animations.h"  // Anim, HashAnimation
#include "PlayerUtil.h"  // PlayerIsBeastRace
#include "Settings.h"  // Settings

#include "RE/Skyrim.h"
#include "REL/Relocation.h"
#include "SKSE/API.h"


namespace Shield
{
	Shield* Shield::GetSingleton()
	{
		static Shield singleton;
		return &singleton;
	}


	void Shield::Clear()
	{
		ISerializableForm::Clear();
		_enchantment.Clear();
	}


	bool Shield::Save(SKSE::SerializationInterface* a_intfc, UInt32 a_type, UInt32 a_version)
	{
		if (!ISerializableForm::Save(a_intfc, a_type, a_version)) {
			return false;
		} else if (!_enchantment.Save(a_intfc)) {
			return false;
		} else {
			return true;
		}
	}


	bool Shield::Load(SKSE::SerializationInterface* a_intfc)
	{
		if (!ISerializableForm::Load(a_intfc)) {
			return false;
		} else if (!_enchantment.Load(a_intfc)) {
			return false;
		} else {
			return true;
		}
	}


	RE::TESObjectARMO* Shield::GetForm()
	{
		return static_cast<RE::TESObjectARMO*>(ISerializableForm::GetForm());
	}

	void Shield::SetEnchantmentForm(UInt32 a_formID)
	{
		_enchantment.SetForm(a_formID);
	}


	RE::EnchantmentItem* Shield::GetEnchantmentForm()
	{
		return _enchantment.GetForm();
	}


	UInt32 Shield::GetEnchantmentFormID()
	{
		return _enchantment.GetFormID();
	}


	RE::EnchantmentItem* Shield::Enchantment::GetForm()
	{
		return static_cast<RE::EnchantmentItem*>(ISerializableForm::GetForm());
	}

	ShieldTaskDelegate::ShieldTaskDelegate(bool a_equip) :
		_equip(a_equip)
	{}


	void ShieldTaskDelegate::Run()
	{
		if (_equip) {
			auto player = RE::PlayerCharacter::GetSingleton();
			if (!player->currentProcess->GetEquippedLeftHand()) {
				ShieldEquipVisitor visitor;
				VisitPlayerInventoryChanges(&visitor);
			}
		} else {
			ShieldUnEquipVisitor visitor;
			VisitPlayerInventoryChanges(&visitor);
		}
	}


	void ShieldTaskDelegate::Dispose()
	{
		delete this;
	}


	DelayedShieldLocator::Visitor::Visitor(UInt32 a_formID) :
		_formID(a_formID)
	{}

	bool ShieldTaskDelegate::ShieldEquipVisitor::Accept(RE::InventoryEntryData* a_entry, SInt32 a_count)
	{
		if (a_entry->GetObject()->GetFormID() == Shield::GetSingleton()->GetFormID()) {
			g_skipAnim = true;
			auto shield = static_cast<RE::TESObjectARMO*>(a_entry->GetObject());
			auto enchantment = Shield::GetSingleton()->GetEnchantmentForm();
			if (enchantment) {
				if (!a_entry->extraLists) {
					return true;
				}
				bool found = false;
				for (auto& xList : *a_entry->extraLists) {
					auto xEnch = xList->GetByType<RE::ExtraEnchantment>();
					if (xEnch && xEnch->enchantment && xEnch->enchantment->GetFormID() == enchantment->GetFormID()) {
						found = true;
						break;
					}
				}
				if (!found) {
					return true;
				}
			}
			auto equipManager = RE::ActorEquipManager::GetSingleton();
			auto player = RE::PlayerCharacter::GetSingleton();
			auto xList = (a_entry->extraLists && !a_entry->extraLists->empty()) ? a_entry->extraLists->front() : 0;
			equipManager->EquipObject(player, shield, xList, 1, shield->GetEquipSlot(), true, false, false);
			return false;
		}
		return true;
	}


	bool ShieldTaskDelegate::ShieldUnEquipVisitor::Accept(RE::InventoryEntryData* a_entry, SInt32 a_count)
	{
		using FirstPersonFlag = RE::BGSBipedObjectForm::FirstPersonFlag;

		if (a_entry->GetObject()->GetFormID() == Shield::GetSingleton()->GetFormID()) {
			if (a_entry->extraLists) {
				for (auto& xList : *a_entry->extraLists) {
					if (xList->HasType(RE::ExtraDataType::kWorn)) {
						auto armor = static_cast<RE::TESObjectARMO*>(a_entry->GetObject());
						if (armor->HasPartOf(FirstPersonFlag::kShield)) {
							auto equipManager = RE::ActorEquipManager::GetSingleton();
							auto player = RE::PlayerCharacter::GetSingleton();
							equipManager->UnequipObject(player, armor, xList, 1, armor->GetEquipSlot(), true, false);
							return false;
						}
					}
				}
			}
		}
		return true;
	}


	DelayedShieldLocator::DelayedShieldLocator(UInt32 a_formID) :
		_formID(a_formID)
	{}


	void DelayedShieldLocator::Run()
	{
		Visitor visitor(_formID);
		VisitPlayerInventoryChanges(&visitor);
	}


	void DelayedShieldLocator::Dispose()
	{
		delete this;
	}


	bool DelayedShieldLocator::Visitor::Accept(RE::InventoryEntryData* a_entry, SInt32 a_count)
	{
		if (a_entry->GetObject()->GetFormID() == _formID && a_entry->extraLists) {
			for (auto& xList : *a_entry->extraLists) {
				if (xList->HasType(RE::ExtraDataType::kWorn)) {
					auto shield = Shield::GetSingleton();
					shield->Clear();
					shield->SetForm(_formID);
					auto armor = static_cast<RE::TESObjectARMO*>(a_entry->GetObject());
					for (auto& xList : *a_entry->extraLists) {
						if (xList->HasType(RE::ExtraDataType::kEnchantment)) {
							auto ench = xList->GetByType<RE::ExtraEnchantment>();
							if (ench && ench->enchantment) {
								shield->SetEnchantmentForm(ench->enchantment->GetFormID());
							}
						}
					}
					return false;
				}
			}
		}
		return true;
	}


	void AnimGraphSinkDelegate::Run()
	{
		SinkAnimationGraphEventHandler(BSAnimationGraphEventHandler::GetSingleton());
	}


	void AnimGraphSinkDelegate::Dispose()
	{
		delete this;
	}


	TESEquipEventHandler* TESEquipEventHandler::GetSingleton()
	{
		static TESEquipEventHandler singleton;
		return &singleton;
	}


	auto TESEquipEventHandler::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource)
		-> EventResult
	{
		using FirstPersonFlag = RE::BGSBipedObjectForm::FirstPersonFlag;

		if (!a_event || !a_event->actor || !a_event->actor->IsPlayerRef() || PlayerIsBeastRace()) {
			return EventResult::kContinue;
		}

		auto armor = RE::TESForm::LookupByID<RE::TESObjectARMO>(a_event->baseObject);
		if (!armor) {
			return EventResult::kContinue;
		}

		if (armor->HasPartOf(FirstPersonFlag::kShield)) {
			auto shield = Shield::GetSingleton();
			if (a_event->equipped) {
				SKSE::GetTaskInterface()->AddTask(new DelayedShieldLocator(armor->GetFormID()));
			} else {
				auto player = RE::PlayerCharacter::GetSingleton();
				if (player->IsWeaponDrawn()) {
					shield->Clear();
				}
			} 
		}

		return EventResult::kContinue;
	}


	BSAnimationGraphEventHandler* BSAnimationGraphEventHandler::GetSingleton()
	{
		static BSAnimationGraphEventHandler singleton;
		return &singleton;
	}


	auto BSAnimationGraphEventHandler::ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
		-> EventResult
	{
		if (!a_event || !a_event->holder || !a_event->holder->IsPlayerRef()) {
			return EventResult::kContinue;
		}

		auto task = SKSE::GetTaskInterface();
		switch (HashAnimation(a_event->tag)) {
		case Anim::kWeaponDraw:
			if (!PlayerIsBeastRace()) {
				task->AddTask(new ShieldTaskDelegate(true));
			}
			break;
		case Anim::kWeaponSheathe:
			if (!PlayerIsBeastRace()) {
				task->AddTask(new ShieldTaskDelegate(false));
			}
			break;
		case Anim::kTailCombatIdle:
			if (!PlayerIsBeastRace()) {
				g_skipAnim = false;
			}
			break;
		case Anim::kGraphDeleting:
			task->AddTask(new AnimGraphSinkDelegate());
			break;
		}

		return EventResult::kContinue;
	}


	class PlayerCharacterEx : public RE::PlayerCharacter
	{
	public:
		using func_t = decltype(&RE::PlayerCharacter::OnItemEquipped);
		static inline REL::Function<func_t> func;


		// This hook prevents a double equip anim bug
		void Hook_OnItemEquipped(bool a_playAnim)			// B2
		{
			if (g_skipAnim) {
				a_playAnim = false;
			}
			func(this, a_playAnim);
		}


		static void InstallHooks()
		{
			REL::Offset<std::uintptr_t> vTable(REL::ID(261916));
			func = vTable.WriteVFunc(0xB2, &RE::PlayerCharacter::OnItemEquipped);
			_DMESSAGE("Installed hooks for (%s)", typeid(PlayerCharacterEx).name());
		}
	};


	void InstallHooks()
	{
		PlayerCharacterEx::InstallHooks();
	}
}
