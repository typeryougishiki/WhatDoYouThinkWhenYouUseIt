#pragma once

namespace hook::ActorEquipManger::EquipObject {
	constexpr auto FUNCTION_ID{ RE::ID::ActorEquipManager::EquipObject };
	struct AMustFuncCallWithActorAndEquipObjectAsArgument {
		//一个以RE::Actor* a_actor和RE::BGSObjectInstance* a_object作为参数的对不知名函数的调用
		//该函数调用在指针a_actor和引用a_object均未指向nullptr的情况下必然被发生
		inline constexpr static std::ptrdiff_t CALL_LOCATION{ 0x1db };//对该函数调用call指令相对于EquipObject函数起始点的偏移量
		static int64_t thunk(int64_t unk1, int64_t unk2, RE::Actor* a_actor, RE::BGSObjectInstance* a_object);
		inline static REL::Relocation<decltype(thunk)> func;
		inline static auto GetOrginialFunc(){return func;}
		inline static void Install() {
			REL::Relocation<std::uintptr_t> target{ FUNCTION_ID, CALL_LOCATION };
			stl::write_thunk_call<AMustFuncCallWithActorAndEquipObjectAsArgument>(target.address());
			spdlog::info("'What do you think when you use it' Use item hook Install Sucess");
		}
	};
}