#include "PlayerUtil.h"

#include <utility>  // pair, make_pair
#include <map>  // map
#include <vector>  // vector

#include "Forms.h"  // WerewolfBeastRace, DLC1VampireBeastRace

#include "RE/Skyrim.h"
#include "SKSE/API.h"


void VisitPlayerInventoryChanges(InventoryChangesVisitor* a_visitor)
{
	const auto player = RE::PlayerCharacter::GetSingleton();
	const auto changes = player->GetInventoryChanges();
	std::map<FormID, std::pair<RE::InventoryEntryData*, Count>> invMap;
	if (changes) {
		for (auto& entry : *changes->entryList) {
			if (entry && entry->GetObject()) {
				invMap.emplace(entry->GetObject()->GetFormID(), std::make_pair(entry, entry->countDelta));
			}
		}
	}

	const auto container = player->GetContainer();
	std::vector<RE::InventoryEntryData*> heapList;
	if (container) {
		container->ForEachContainerObject([&](RE::ContainerObject* a_cnto) -> bool
		{
			if (a_cnto->obj) {
				auto& it = invMap.find(a_cnto->obj->GetFormID());
				if (it != invMap.end()) {
					if (!a_cnto->obj->IsGold()) {
						it->second.second += a_cnto->count;
					}
				} else {
					auto entryData = new RE::InventoryEntryData(a_cnto->obj, a_cnto->count);
					heapList.push_back(entryData);
					invMap.emplace(a_cnto->obj->GetFormID(), std::make_pair(entryData, entryData->countDelta));
				}
			}
			return true;
		});
	}

	for (auto& item : invMap) {
		if (item.second.second > 0) {
			if (!a_visitor->Accept(item.second.first, item.second.second)) {
				break;
			}
		}
	}

	for (auto& entry : heapList) {
		delete entry;
	}
}


bool AnimationGraphEventHandler(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_sink)
{
	const auto player = RE::PlayerCharacter::GetSingleton();
	RE::BSAnimationGraphManagerPtr graphManager;
	player->GetAnimationGraphManager(graphManager);
	if (graphManager) {
		auto sinked = false;
		for (auto& animationGraph : graphManager->graphs) {
			if (sinked) {
				break;
			}

			auto eventSource = animationGraph->GetEventSource<RE::BSAnimationGraphEvent>();
			for (auto& sink : eventSource->sinks) {
				if (sink == a_sink) {
					sinked = true;
					break;
				}
			}
		}

		if (!sinked) {
			graphManager->graphs.front()->GetEventSource<RE::BSAnimationGraphEvent>()->AddEventSink(a_sink);
			return true;
		}
	}
	return false;
}


bool PlayerIsBeastRace()
{
	const auto player = RE::PlayerCharacter::GetSingleton();
	const auto race = player->GetRace();
	return race == WerewolfBeastRace || race == DLC1VampireBeastRace;
}
