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
	static inline std::atomic<bool> Inited =false;
	using MapType = str_key_allow_str_view_find_unordered<char>::map<ThinkData>;
	MapType IndexByName;
	MapType IndexByEditorId;
	MapType IndexByLocalName;
	inline constexpr MapType* GetMapPtrByIndexType(const std::string_view & index_type) {
		if (index_type == "Name"sv) {
			return std::addressof(IndexByName);
		}else if (index_type == "EditorId"sv) {
			return std::addressof(IndexByEditorId);
		}
		else if(index_type == "LocalName"sv) {
			return std::addressof(IndexByLocalName);
		}
		return nullptr;
	}
	inline static void AddToThinkDatasSingleton(const rapidcsv::Document& v,bool is_final) {
		auto count = v.GetRowCount();
		for (size_t i = 0; i < count; ++i) {
			auto row{ v.GetRow<std::string>(i) };
			if (row.size() < 3) {
				logger::warn("Row {} cell num < 3.", i + 1);
				continue;
			}
			std::string_view index_type{ row.at(1) };
			auto ThinkDataMapPtr = ThinkDatas::GetSingleton()->GetMapPtrByIndexType(index_type);
			if (ThinkDataMapPtr == nullptr) {
				logger::error("Row {} index type '{}' not support.", i + 1,index_type);
				continue;
			}
			std::string& think{ row.at(0) }, index{ row.at(2) };
			if (index.empty()) {
				logger::error("Row {} index type '{}' index empty.", i + 1, index_type);
				continue;
			}
			if (think.empty()) {
				logger::error("Row {} think empty.", i + 1);
				continue;
			}
			auto find_result = ThinkDataMapPtr->find(index);
			if(find_result == ThinkDataMapPtr->end()){
				auto statu = ThinkDataMapPtr->emplace(std::move(index), std::move(think));
				find_result = statu.first;
			}
			else {
				if (!is_final) logger::warn("Not in final file,Row {} redefine index {}:{} < 3.", i + 1,index_type,index);
				find_result->second = ThinkData(std::move(think));
			}
			for (size_t j = 3; j + 1 < row.size(); j += 2) {
				find_result->second.AddCondition(row.at(j), row.at(j + 1));
			}
		}
	}
};



