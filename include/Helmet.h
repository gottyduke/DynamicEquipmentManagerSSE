#pragma once

#include "skse64/gamethreads.h" // TaskDelegate

#include "ISerializableForm.h"  // ISerializableForm
#include "PlayerUtil.h"  // InventoryChangesVisitor

#include "RE/Skyrim.h"


namespace Helmet
{
	class Helmet : public ISerializableForm
	{
	public:
		static Helmet* GetSingleton();

		void Clear();
		bool Save(SKSE::SerializationInterface* a_intfc, UInt32 a_type, UInt32 a_version) const;
		bool Load(SKSE::SerializationInterface* a_intfc);

		[[nodiscard]] RE::TESObjectARMO* GetForm() const;
		void SetEnchantmentForm(UInt32 a_formID);

		[[nodiscard]] RE::EnchantmentItem* GetEnchantmentForm() const;
		[[nodiscard]] UInt32 GetEnchantmentFormID() const;

		Helmet& operator=(const Helmet&) = delete;
		Helmet& operator=(Helmet&&) = delete;

		Helmet(const Helmet&) = delete;
		Helmet(Helmet&&) = delete;

	protected:
		class Enchantment : public ISerializableForm
		{
		public:
			Enchantment() = default;
			~Enchantment() = default;

			[[nodiscard]] RE::EnchantmentItem* GetForm() const;
		};


		Helmet() = default;
		~Helmet() = default;

		Enchantment _enchantment;
	};


	class HelmetTaskDelegate : public TaskDelegate
	{
	public:
		class HelmetEquipVisitor : public InventoryChangesVisitor
		{
		public:
			bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		class HelmetUnEquipVisitor : public InventoryChangesVisitor
		{
		public:
			bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		explicit HelmetTaskDelegate(bool a_equip);
		~HelmetTaskDelegate() = default;

		void Run() override;
		void Dispose() override;

	private:
		bool _equip;
	};


	class DelayedHelmetLocator : public TaskDelegate
	{
	public:
		class Visitor : public InventoryChangesVisitor
		{
		public:
			explicit Visitor(UInt32 a_formID);
			virtual ~Visitor() = default;

			bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;

		private:
			UInt32 _formID;
		};


		explicit DelayedHelmetLocator(UInt32 a_formID);
		~DelayedHelmetLocator() = default;

		void Run() override;
		void Dispose() override;

	private:
		UInt32 _formID;
	};


	class AnimGraphSinkDelegate : public TaskDelegate
	{
	public:
		void Run() override;
		void Dispose() override;
	};


	class TESEquipEventHandler final : public RE::BSTEventSink<RE::TESEquipEvent>
	{
	public:
		using EventResult = RE::BSEventNotifyControl;

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


	class BSAnimationGraphEventHandler final : public RE::BSTEventSink<RE::BSAnimationGraphEvent>
	{
	public:
		using EventResult = RE::BSEventNotifyControl;

		static BSAnimationGraphEventHandler* GetSingleton();
		EventResult ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) override;

		BSAnimationGraphEventHandler(const BSAnimationGraphEventHandler&) = delete;
		BSAnimationGraphEventHandler(BSAnimationGraphEventHandler&&) = delete;

		BSAnimationGraphEventHandler& operator=(const BSAnimationGraphEventHandler&) = delete;
		BSAnimationGraphEventHandler& operator=(BSAnimationGraphEventHandler&&) = delete;

	protected:
		BSAnimationGraphEventHandler() = default;
		virtual ~BSAnimationGraphEventHandler() = default;
	};
}
