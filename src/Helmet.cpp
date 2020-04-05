#include "Helmet.h"

#include <type_traits>  // typeid

#include "Animations.h"  // Anim, HashAnimation
#include "Forms.h"  // WerewolfBeastRace, DLC1VampireBeastRace

#include "RE/Skyrim.h"
#include "SKSE/API.h"


namespace Helmet
{
	auto Helmet::GetSingleton()
	-> Helmet*
	{
		static Helmet singleton;
		return std::addressof(singleton);
	}


	void Helmet::Clear()
	{
		ISerializableForm::Clear();
		_enchantment.Clear();
	}


	bool Helmet::Save(SKSE::SerializationInterface* a_intfc, const UInt32 a_type, const UInt32 a_version) const
	{
		if (!ISerializableForm::Save(a_intfc, a_type, a_version)) {
			return false;
		}
		if (!_enchantment.Save(a_intfc)) {
			return false;
		}
		return true;
	}


	bool Helmet::Load(SKSE::SerializationInterface* a_intfc)
	{
		if (!ISerializableForm::Load(a_intfc)) {
			return false;
		}
		if (!_enchantment.Load(a_intfc)) {
			return false;
		}
		return true;
	}


	auto Helmet::GetForm() const
	-> RE::TESObjectARMO*
	{
		return static_cast<RE::TESObjectARMO*>(ISerializableForm::GetForm());
	}


	void Helmet::SetEnchantmentForm(const UInt32 a_formID)
	{
		_enchantment.SetForm(a_formID);
	}


	auto Helmet::GetEnchantmentForm() const
	-> RE::EnchantmentItem*
	{
		return _enchantment.GetForm();
	}


	UInt32 Helmet::GetEnchantmentFormID() const
	{
		return _enchantment.GetFormID();
	}


	auto Helmet::Enchantment::GetForm() const
	-> RE::EnchantmentItem*
	{
		return static_cast<RE::EnchantmentItem*>(ISerializableForm::GetForm());
	}


	HelmetTaskDelegate::HelmetTaskDelegate(const bool a_equip) :
		_equip(a_equip)
	{}


	void HelmetTaskDelegate::Run()
	{
		if (_equip) {
			HelmetEquipVisitor visitor;
			VisitPlayerInventoryChanges(&visitor);
		} else {
			HelmetUnEquipVisitor visitor;
			VisitPlayerInventoryChanges(&visitor);
		}
	}


	void HelmetTaskDelegate::Dispose()
	{
		delete this;
	}


	DelayedHelmetLocator::Visitor::Visitor(const UInt32 a_formID) :
		_formID(a_formID)
	{}


	bool HelmetTaskDelegate::HelmetEquipVisitor::Accept(RE::InventoryEntryData* a_entry, SInt32 a_count)
	{
		const auto helmet = Helmet::GetSingleton();
		if (a_entry->GetObject()->GetFormID() == helmet->GetFormID()) {
			const auto armor = static_cast<RE::TESObjectARMO*>(a_entry->GetObject());
			const auto enchantment = helmet->GetEnchantmentForm();
			if (enchantment) {
				if (!a_entry->extraLists) {
					return true;
				}
				auto found = false;
				for (auto& xList : *a_entry->extraLists) {
					const auto xEnch = xList->GetByType<RE::ExtraEnchantment>();
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
			const auto player = RE::PlayerCharacter::GetSingleton();
			const auto xList = (a_entry->extraLists && !a_entry->extraLists->empty()) ? a_entry->extraLists->front() : nullptr;
			equipManager->EquipObject(player, armor, xList, 1, armor->equipSlot, true, false, false);
			return false;
		}
		return true;
	}


	bool HelmetTaskDelegate::HelmetUnEquipVisitor::Accept(RE::InventoryEntryData* a_entry, SInt32 a_count)
	{
		using FirstPersonFlag = RE::BGSBipedObjectForm::FirstPersonFlag;

		if (a_entry->GetObject() && a_entry->GetObject()->Is(RE::FormType::Armor) && a_entry->extraLists) {
			for (auto& xList : *a_entry->extraLists) {
				if (xList->HasType(RE::ExtraDataType::kWorn)) {
					const auto armor = static_cast<RE::TESObjectARMO*>(a_entry->GetObject());
					if (armor->HasPartOf(FirstPersonFlag::kHair) && (armor->IsLightArmor() || armor->IsHeavyArmor() || armor->IsClothing())) {
						auto equipManager = RE::ActorEquipManager::GetSingleton();
						const auto player = RE::PlayerCharacter::GetSingleton();
						equipManager->UnequipObject(player, armor, xList, 1, armor->equipSlot, true, false);
						return false;
					}
				}
			}
		}
		return true;
	}


	DelayedHelmetLocator::DelayedHelmetLocator(const UInt32 a_formID) :
		_formID(a_formID)
	{}


	void DelayedHelmetLocator::Run()
	{
		Visitor visitor(_formID);
		VisitPlayerInventoryChanges(&visitor);
	}


	void DelayedHelmetLocator::Dispose()
	{
		delete this;
	}


	bool DelayedHelmetLocator::Visitor::Accept(RE::InventoryEntryData* a_entry, SInt32 a_count)
	{
		if (a_entry->GetObject()->GetFormID() == _formID && a_entry->extraLists) {
			for (auto& xList : *a_entry->extraLists) {
				if (xList->HasType(RE::ExtraDataType::kWorn)) {
					auto helmet = Helmet::GetSingleton();
					helmet->Clear();
					helmet->SetForm(_formID);
					auto armor = static_cast<RE::TESObjectARMO*>(a_entry->GetObject());
					for (auto& xList : *a_entry->extraLists) {
						if (xList->HasType(RE::ExtraDataType::kEnchantment)) {
							const auto ench = xList->GetByType<RE::ExtraEnchantment>();
							if (ench && ench->enchantment) {
								helmet->SetEnchantmentForm(ench->enchantment->GetFormID());
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

		if (armor->HasPartOf(FirstPersonFlag::kHead | FirstPersonFlag::kHair | FirstPersonFlag::kCirclet)) {
			auto helmet = Helmet::GetSingleton();
			if (armor->IsLightArmor() || armor->IsHeavyArmor() || armor->IsClothing()) {
				if (a_event->equipped) {
					SKSE::GetTaskInterface()->AddTask(new DelayedHelmetLocator(armor->GetFormID()));
				} else {
					const auto player = RE::PlayerCharacter::GetSingleton();
					if (player->IsWeaponDrawn()) {
						helmet->Clear();
					}
				}
			} else {
				helmet->Clear();
			}
		}

		return EventResult::kContinue;
	}


	BSAnimationGraphEventHandler* BSAnimationGraphEventHandler::GetSingleton()
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
			{
				if (!PlayerIsBeastRace()) {
					task->AddTask(new HelmetTaskDelegate(true));
				}
				break;
			}
		case Anim::kWeaponSheathe:
			{
				if (!PlayerIsBeastRace()) {
					task->AddTask(new HelmetTaskDelegate(false));
				}
				break;
			}
		case Anim::kGraphDeleting:
			{
				task->AddTask(new AnimGraphSinkDelegate());
				break;
			}
		default: ;
		}

		return EventResult::kContinue;
	}
}
