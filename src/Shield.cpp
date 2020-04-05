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
	auto Shield::GetSingleton()
	-> Shield*
	{
		static Shield singleton;
		return std::addressof(singleton);
	}


	auto Shield::GetForm() const
	-> RE::TESObjectARMO*
	{
		return static_cast<RE::TESObjectARMO*>(ISerializableForm::GetForm());
	}


	ShieldTaskDelegate::ShieldTaskDelegate(const bool a_equip) :
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


	bool ShieldTaskDelegate::ShieldEquipVisitor::Accept(RE::InventoryEntryData* a_entry, SInt32 a_count)
	{
		if (a_entry->GetObject()->GetFormID() == Shield::GetSingleton()->GetFormID()) {
			g_skipAnim = true;
			const auto shield = static_cast<RE::TESObjectARMO*>(a_entry->GetObject());
			auto equipManager = RE::ActorEquipManager::GetSingleton();
			const auto player = RE::PlayerCharacter::GetSingleton();
			const auto xList = (a_entry->extraLists && !a_entry->extraLists->empty()) ? a_entry->extraLists->front() : nullptr;
			equipManager->EquipObject(player, shield, xList, 1, shield->equipSlot, true, false, false);
			return false;
		}
		return true;
	}


	bool ShieldTaskDelegate::ShieldUnEquipVisitor::Accept(RE::InventoryEntryData* a_entry, SInt32 a_count)
	{
		using FirstPersonFlag = RE::BGSBipedObjectForm::FirstPersonFlag;

		if (a_entry->GetObject() && a_entry->GetObject()->Is(RE::FormType::Armor) &&
			a_entry->GetObject()->GetFormID() == Shield::GetSingleton()->GetFormID()) {
			if (a_entry->extraLists) {
				for (auto& xList : *a_entry->extraLists) {
					if (xList->HasType(RE::ExtraDataType::kWorn)) {
						const auto armor = static_cast<RE::TESObjectARMO*>(a_entry->GetObject());
						if (armor->HasPartOf(FirstPersonFlag::kShield)) {
							auto equipManager = RE::ActorEquipManager::GetSingleton();
							const auto player = RE::PlayerCharacter::GetSingleton();
							equipManager->UnequipObject(player, armor, xList, 1, armor->equipSlot, true, false);
							return false;
						}
					}
				}
			}
		}
		return true;
	}


	void AnimGraphSinkDelegate::Run()
	{
		AnimationGraphEventHandler(BSAnimationGraphEventHandler::GetSingleton());
	}


	void AnimGraphSinkDelegate::Dispose()
	{
		delete this;
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
		using FirstPersonFlag = RE::BGSBipedObjectForm::FirstPersonFlag;

		if (!a_event || !a_event->actor || !a_event->actor->IsPlayerRef() || PlayerIsBeastRace()) {
			return EventResult::kContinue;
		}

		const auto armor = RE::TESForm::LookupByID<RE::TESObjectARMO>(a_event->baseObject);
		if (!armor) {
			return EventResult::kContinue;
		}

		if (armor->HasPartOf(FirstPersonFlag::kShield)) {
			auto shield = Shield::GetSingleton();
			if (a_event->equipped) {
				shield->SetForm(a_event->baseObject);
			} else {
				const auto player = RE::PlayerCharacter::GetSingleton();
				if (player->IsWeaponDrawn()) {
					shield->Clear();
				}
			}
		}

		return EventResult::kContinue;
	}


	auto BSAnimationGraphEventHandler::GetSingleton()
	-> BSAnimationGraphEventHandler*
	{
		static BSAnimationGraphEventHandler singleton;
		return std::addressof(singleton);
	}


	auto BSAnimationGraphEventHandler::ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource)
	-> EventResult
	{
		if (!a_event || !a_event->holder || !a_event->holder->IsPlayerRef()) {
			return EventResult::kContinue;
		}

		const auto task = SKSE::GetTaskInterface();
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
		using func_t = decltype(&PlayerCharacter::OnItemEquipped);
		static inline REL::Function<func_t> func;


		// This hook prevents a double equip anim bug
		void Hook_OnItemEquipped(bool a_playAnim) // B2
		{
			if (g_skipAnim) {
				a_playAnim = false;
			}
			func(this, a_playAnim);
		}


		static void InstallHooks()
		{
			REL::Offset<std::uintptr_t> vTable(REL::ID(261916));
			func = vTable.WriteVFunc(0xB2, &PlayerCharacter::OnItemEquipped);
			_DMESSAGE("Installed hooks for (%s)", typeid(PlayerCharacterEx).name());
		}
	};


	void InstallHooks()
	{
		PlayerCharacterEx::InstallHooks();
	}
}
