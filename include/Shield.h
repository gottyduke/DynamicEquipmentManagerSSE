#pragma once

#include "skse64/gamethreads.h"  // TaskDelegate

#include "ISerializableForm.h"  // ISerializableForm
#include "PlayerUtil.h"  // InventoryChangesVisitor

#include "RE/Skyrim.h"
#include "SKSE/API.h"


namespace Shield
{
	class Shield : public ISerializableForm
	{
	public:
		static Shield* GetSingleton();

		[[nodiscard]] RE::TESObjectARMO* GetForm() const;

		Shield(const Shield&) = delete;
		Shield(Shield&&) = delete;

		Shield& operator=(const Shield&) = delete;
		Shield& operator=(Shield&&) = delete;

	protected:
		Shield() = default;
		~Shield() = default;
	};


	class ShieldTaskDelegate final : public TaskDelegate
	{
	public:
		class ShieldEquipVisitor : public InventoryChangesVisitor
		{
		public:
			bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		class ShieldUnEquipVisitor : public InventoryChangesVisitor
		{
		public:
			bool Accept(RE::InventoryEntryData* a_entry, SInt32 a_count) override;
		};


		explicit ShieldTaskDelegate(bool a_equip);
		virtual ~ShieldTaskDelegate() = default;

		void Run() override;
		void Dispose() override;

	private:
		bool _equip;
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


	void InstallHooks();


	namespace
	{
		bool g_skipAnim = false;
	}
}
