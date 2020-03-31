#pragma once

#include "skse64/gamethreads.h" // TaskDelegate

#include "ISerializableForm.h"  // ISerializableForm
#include "PlayerUtil.h"  // InventoryChangesVisitor

#include "RE/Skyrim.h"
#include "SKSE/API.h"

namespace Helmet
{
	class Helmet : public ISerializableForm
	{
	public:
		static Helmet* GetSingleton();

		void Clear();
		bool Save(SKSE::SerializationInterface* a_intfc, UInt32 a_type, UInt32 a_version);
		bool Load(SKSE::SerializationInterface* a_intfc);

		RE::TESObjectARMO* GetForm();
		void SetEnchantmentForm(UInt32 a_formID);

		RE::EnchantmentItem* GetEnchantmentForm();
		UInt32 GetEnchantmentFormID();

	protected:
		class Enchantment : public ISerializableForm
		{
		public:
			Enchantment() = default;
			~Enchantment() = default;

			RE::EnchantmentItem* GetForm();
		};


		Helmet() = default;
		Helmet(const Helmet&) = delete;
		Helmet(Helmet&&) = delete;
		~Helmet() = default;

		Helmet& operator=(const Helmet&) = delete;
		Helmet& operator=(Helmet&&) = delete;


		Enchantment _enchantment;
	};


	class HelmetTaskDelegate : public TaskDelegate
	{
	public:
		class HelmetEquipVisitor : public InventoryChangesVisitor
		{
		public:
			virtual bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		class HelmetUnEquipVisitor : public InventoryChangesVisitor
		{
		public:
			virtual bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		explicit HelmetTaskDelegate(bool a_equip);
		~HelmetTaskDelegate() = default;

		virtual void Run() override;
		virtual void Dispose() override;

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

			virtual bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;

		private:
			UInt32 _formID;
		};


		explicit DelayedHelmetLocator(UInt32 a_formID);
		~DelayedHelmetLocator() = default;

		virtual void Run() override;
		virtual void Dispose() override;

	private:
		UInt32 _formID;
	};


	class AnimGraphSinkDelegate : public TaskDelegate
	{
	public:
		virtual void Run() override;
		virtual void Dispose() override;
	};


	class TESEquipEventHandler : public RE::BSTEventSink<RE::TESEquipEvent>
	{
	public:
		using EventResult = RE::BSEventNotifyControl;

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


	class BSAnimationGraphEventHandler : public RE::BSTEventSink<RE::BSAnimationGraphEvent>
	{
	public:
		using EventResult = RE::BSEventNotifyControl;

		static BSAnimationGraphEventHandler* GetSingleton();
		virtual EventResult ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) override;

	protected:
		BSAnimationGraphEventHandler() = default;
		BSAnimationGraphEventHandler(const BSAnimationGraphEventHandler&) = delete;
		BSAnimationGraphEventHandler(BSAnimationGraphEventHandler&&) = delete;
		virtual ~BSAnimationGraphEventHandler() = default;

		BSAnimationGraphEventHandler& operator=(const BSAnimationGraphEventHandler&) = delete;
		BSAnimationGraphEventHandler& operator=(BSAnimationGraphEventHandler&&) = delete;
	};
}
