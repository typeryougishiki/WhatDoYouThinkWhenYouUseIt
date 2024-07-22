#include "ActorEquipManager.h"
#include "ThinkDatas.h"
#include "utils/Config.h"

inline static std::optional<RE::TESFullName*> TryGetItemNameCompentFrom(RE::TESForm* item_form) {
	switch (item_form->GetFormType()){
#define TryGetItemNameCompentFrom_CASE_WITH_CHECK(FORM_TYPE,FORM_CLASS)\
	case FORM_TYPE :{\
		static_assert(FORM_TYPE == FORM_CLASS::FORMTYPE);\
		return item_form->As<FORM_CLASS>();\
	}break;

	TryGetItemNameCompentFrom_CASE_WITH_CHECK(RE::FormType::kALCH, RE::AlchemyItem);
	TryGetItemNameCompentFrom_CASE_WITH_CHECK(RE::FormType::kWEAP, RE::TESObjectWEAP);
	TryGetItemNameCompentFrom_CASE_WITH_CHECK(RE::FormType::kARMO, RE::TESObjectARMO);
	TryGetItemNameCompentFrom_CASE_WITH_CHECK(RE::FormType::kAMMO, RE::TESAmmo);
	TryGetItemNameCompentFrom_CASE_WITH_CHECK(RE::FormType::kBOOK, RE::TESObjectBOOK);
#undef TryGetItemNameCompentFrom_CASE_WITH_CHECK
	default: {
		logger::warn("UnImplement Form Type:{}"sv, RE::FormTypeToString(item_form->GetFormType()));
		return std::nullopt;
	}break;
	}
}

int64_t hook::ActorEquipManger::EquipObject::AMustFuncCallWithActorAndEquipObjectAsArgument::thunk(int64_t unk1, int64_t unk2, RE::Actor* a_actor, RE::BGSObjectInstance* a_object) {
	auto actor_reference = a_actor->AsReference();
	if (ThinkDatas::Inited && actor_reference->IsPlayerRef()) [[unlikely]] {
		auto item_form = a_object->object;
		std::optional<std::string_view> think{std::nullopt};
		std::optional<RE::TESFullName*> item_name_compment{ TryGetItemNameCompentFrom(item_form)};
		std::array<std::function<std::optional<std::string_view>()>, ThinkDatas::index_type_num> index_getters{
			[&item_form]()->auto {
				if (item_form->GetFormEditorID() != nullptr && item_form->GetFormEditorIDLength() != 0) {
					return std::make_optional<std::string_view>(item_form->GetFormEditorID(), item_form->GetFormEditorIDLength());
				}
				return std::optional<std::string_view>(std::nullopt);
			},
			[&item_name_compment]()->auto {
				auto p = item_name_compment.value_or(nullptr);
				if (p != nullptr) {
					auto& localed_result = p->GetFullNameLocalized();
					if (localed_result.c_str() != nullptr && localed_result.size() != 0) {
						return std::make_optional<std::string_view>(localed_result.c_str(), localed_result.size());
					}
				}
				return std::optional<std::string_view>(std::nullopt);
			}
		};

		//RE::DebugNotification(std::format("EditorId:{},LocalName:{}"sv, 
		//	index_getters[0]().value_or(""sv), 
		//	index_getters[1]().value_or(""sv)
		//).data());
		for (size_t i = 0; i < ThinkDatas::index_types.size();++i) {
			if (!think.has_value()) {
				const auto* m = ThinkDatas::GetSingleton() ->GetMapPtrByIndexType(ThinkDatas::index_types[i]);
				auto index = index_getters[i]();
				auto find_result = m->find(index_getters[i]().value_or(""sv));
				if (find_result == m->end()) continue;
				if (!find_result->second.CheckCondition(a_actor, a_object))continue;
				think.emplace(find_result->second.think);
				break;
			}
		}
		if (think.has_value()) {
			std::string fmt_think = std::format("{}:{}"sv, Config::PersonalPronous, think.value().data());
			RE::DebugNotification(fmt_think.c_str());
		}
	}
	return GetOrginialFunc()(unk1, unk2, a_actor, a_object);
}
