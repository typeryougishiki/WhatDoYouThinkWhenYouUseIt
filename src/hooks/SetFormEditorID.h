#pragma once
#include"AdditionEditorIDMap.h"
namespace hook {
	struct SetFormEditorID {
		template<typename HookTargetType>
		struct sub {
			inline static bool thunk(RE::TESForm* a_this, const char* a_str) {
				spdlog::info("at {:x}", func.offset());
				spdlog::info("{},{},{}", typeid(*a_this).name(), RE::FormTypeToString(a_this->GetFormType()), a_this->GetFormID());
				if (a_this->formID != 0 && a_str != nullptr && std::strlen(a_str) != 0) {
					auto id = a_this->formID;
					auto& safe_obj = AdditionEditorIDMap::GetSingleton();
					safe_obj.NotConstAccess([id, a_str](std::unordered_map<uint32_t, std::string>& m)->void {
						m.emplace(id, a_str);
						});
					spdlog::info("{}:{}", id, a_str);
				}
				return func(a_this, a_str);
			}
			inline static REL::Relocation<decltype(thunk)> func;
			inline static constexpr size_t idx{ 0x4E };
			inline static void Install() {
				stl::write_vfunc<HookTargetType, SetFormEditorID::sub<HookTargetType>>();
			}
		};
		inline static void Install() {
			SetFormEditorID::sub<RE::AlchemyItem>::Install();
			spdlog::info("'What do you think when you use it' Editor Load hook for AlchemyItem Install Sucess");
		}
	};
}