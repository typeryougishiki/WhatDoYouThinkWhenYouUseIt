#pragma once
#include "utils/str_key_allow_str_view_find_unordered.h"
#include "rapidcsv.h"
struct ThinkData{
	std::string think;
	inline explicit ThinkData(const std::string &think_arg) :think(think_arg) {};
	inline explicit ThinkData(std::string &&think_arg) :think(think_arg) {};
	inline bool CheckCondition(RE::Actor* /*a_actor*/, RE::BGSObjectInstance* /*a_object*/) const{
		return true;
	}
	inline void AddCondition(const std::string_view & /*key*/, const std::string_view& /*value*/){}
};
struct ThinkDatas:Singleton<ThinkDatas> {
	static constexpr auto index_type_num = 2;
	static constexpr std::array<std::string_view, index_type_num> index_types{ "EditorId"sv,"LocalName"sv };
	static inline std::atomic<bool> Inited =false;
	using MapType = str_key_allow_str_view_find_unordered<char>::map<ThinkData>;
	MapType IndexByEditorId;
	MapType IndexByLocalName;
	inline constexpr MapType* GetMapPtrByIndexType(const std::string_view & index_type) {
		if (index_type == "EditorId"sv) {
			return std::addressof(IndexByEditorId);
		}
		else if(index_type == "LocalName"sv) {
			return std::addressof(IndexByLocalName);
		}
		return nullptr;
	}
	inline static void AddToThinkDatasSingleton(const rapidcsv::Document& v,bool is_final) {
		constexpr size_t think_loc{ 0 }, index_type_loc{ 1 }, index_loc{ 2 },condition_start_loc{6};
		[[maybe_unused]] constexpr size_t allow_mult_rand_loc{ 3 }; 
		[[maybe_unused]] constexpr size_t  unused_2_loc{ 4 };
		[[maybe_unused]] constexpr size_t  unused_3_loc{ 5 };
		auto count = v.GetRowCount();
		for (size_t i = 0; i < count; ++i) {
			auto row{ v.GetRow<std::string>(i) };
			if (row.size() < index_loc+1) {
				logger::warn("Row {} cell num < {}."sv, i + 1, index_loc + 1);
				continue;
			}
			std::string_view index_type{ row.at(index_type_loc) };
			auto ThinkDataMapPtr = ThinkDatas::GetSingleton()->GetMapPtrByIndexType(index_type);
			if (ThinkDataMapPtr == nullptr) {
				logger::error("Row {} index type '{}' not support."sv, i + 1,index_type);
				continue;
			}
			std::string& think{ row.at(think_loc) }, index{ row.at(index_loc) };
			if (index.empty()) {
				logger::error("Row {} index type '{}' index empty."sv, i + 1, index_type);
				continue;
			}
			if (think.empty()) {
				logger::error("Row {} think empty."sv, i + 1);
				continue;
			}
			auto find_result = ThinkDataMapPtr->find(index);
			if(find_result == ThinkDataMapPtr->end()){
				auto statu = ThinkDataMapPtr->emplace(std::move(index), std::move(think));
				find_result = statu.first;
			}
			else {
				if (!is_final) logger::warn("Not in final file,Row {} redefine index {}:{} ."sv, i + 1,index_type,index);
				find_result->second = ThinkData(std::move(think));
			}
			for (size_t j = condition_start_loc; j + 1 < row.size(); j += 2) {
				find_result->second.AddCondition(row.at(j), row.at(j + 1));
			}
		}
	}
};



