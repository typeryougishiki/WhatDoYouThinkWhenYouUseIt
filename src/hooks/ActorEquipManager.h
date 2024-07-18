#pragma once

namespace hook::ActorEquipManger::EquipObject {
	constexpr auto FUNCTION_ID{ RE::ID::ActorEquipManager::EquipObject };
	struct AMustFuncCallWithActorAndEquipObjectAsArgument {
		//һ����RE::Actor* a_actor��RE::BGSObjectInstance* a_object��Ϊ�����ĶԲ�֪�������ĵ���
		//�ú���������ָ��a_actor������a_object��δָ��nullptr������±�Ȼ������
		inline constexpr static std::ptrdiff_t CALL_LOCATION{ 0x1db };//�Ըú�������callָ�������EquipObject������ʼ���ƫ����
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