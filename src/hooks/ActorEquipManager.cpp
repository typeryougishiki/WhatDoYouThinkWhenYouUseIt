#include "ActorEquipManager.h"
#include "ThinkDatas.h"
#include "utils/Config.h"
int64_t hook::ActorEquipManger::EquipObject::AMustFuncCallWithActorAndEquipObjectAsArgument::thunk(int64_t unk1, int64_t unk2, RE::Actor* a_actor, RE::BGSObjectInstance* a_object) {
	auto actor_reference = a_actor->AsReference();
	if (ThinkDatas::Inited && actor_reference->IsPlayerRef()) [[unlikely]] {
		auto item_form = a_object->object;
		std::optional<std::string_view> think{std::nullopt};
		constexpr std::array<std::string_view,3> index_types{ "EditorId"sv,"Name"sv,"LocalName"sv };
		std::array<std::function<std::optional<std::string_view>()>, 3> index_getters{
			[&item_form]()->auto {
				if (item_form->GetFormEditorID() != nullptr && item_form->GetFormEditorIDLength() != 0) {
					return std::make_optional<std::string_view>(item_form->GetFormEditorID(), item_form->GetFormEditorIDLength());
				}
				return std::optional<std::string_view>(std::nullopt);
			},
			[&item_form]()->auto {
				auto* item_name_compment = dynamic_cast<RE::TESFullName*>(item_form);
				if (item_name_compment != nullptr) {
					if (item_name_compment->GetFullName() != nullptr && item_name_compment->GetFullNameLength() != 0) {
						return std::make_optional<std::string_view>(item_name_compment->GetFullName(), item_name_compment->GetFullNameLength());
					}
				}
				return std::optional<std::string_view>(std::nullopt);
			},
			[&item_form]()->auto {
				auto* item_name_compment = dynamic_cast<RE::TESFullName*>(item_form);
				if (item_name_compment != nullptr) {
					auto& localed_result = item_name_compment->GetFullNameLocalized();
					if (localed_result.c_str() != nullptr && localed_result.size() != 0) {
						return std::make_optional<std::string_view>(localed_result.c_str(), localed_result.size());
					}
				}
				return std::optional<std::string_view>(std::nullopt);
			}
		};
		for (size_t i = 0; i < index_types.size();++i) {
			if (!think.has_value()) {
				const auto* m = ThinkDatas::GetSingleton() ->GetMapPtrByIndexType(index_types[i]);
				auto index = index_getters[i]();
				auto find_result = m->find(index_getters[i]().value_or(""sv));
				if (find_result == m->end()) continue;
				if (!find_result->second.CheckCondition(a_actor, a_object))continue;
				think.emplace(find_result->second.think);
				break;
			}
		}
		if (think.has_value()) {
			RE::DebugNotification(think.value().data());
		}
	}
	return GetOrginialFunc()(unk1, unk2, a_actor, a_object);
}
