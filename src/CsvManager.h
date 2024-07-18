#pragma once
#include "rapidcsv.h"
#include "utils/ThreadSafeObjPkg.h"
#include "utils/str_key_allow_str_view_find_unordered.h"
class CsvManager {
public:
private:
    mutable std::shared_mutex mutex;
	str_key_allow_str_view_find_unordered<char>::map<ThreadSafeObjPkj<rapidcsv::Document>> csv_table;
    std::vector<std::string_view> configured_csv_names;
	str_key_allow_str_view_find_unordered<char>::map<std::vector<decltype(csv_table)::const_iterator>> prefix_indexs;
	static void ExceptionWhenCsvNameNotConfigure(const std::string_view& p) {
		spdlog::error("Try to find csv '{}' which not be configured,failed", p);
		throw std::out_of_range(std::format("Csv file path '{}' not configure ", p));
	}

public:
    static constexpr std::string_view BASE_CSV_DIR_PATH{ "./Data/SFSE/Plugins/WhatDoYouThinkWhenYouUseIt" };
	inline CsvManager() {
		std::filesystem::path base_csv_dir{ BASE_CSV_DIR_PATH };
		if (!std::filesystem::exists(base_csv_dir)) {
			std::filesystem::create_directories(base_csv_dir);
			spdlog::warn("Csv store directory {} not found, create it.", base_csv_dir.string());
			return;
		}
		for (auto& file : std::filesystem::directory_iterator(base_csv_dir)) {
			if (!(file.is_regular_file() && file.path().extension() == ".csv")) [[unlikely]] continue;
			std::string csv_name{ file.path().filename().string() };
			try {
				auto csv_uptr = std::make_unique<rapidcsv::Document>(file.path().string());
				auto p = this->csv_table.emplace(csv_name, std::move(csv_uptr));
				this->configured_csv_names.emplace_back(p.first->first);
				spdlog::info("Configure csv {}", csv_name);
			}
			catch (const std::iostream::failure& e) {
				spdlog::warn("Try to open file \"{}\" ,failed: {}", file.path().filename().string(), e.what());
			}
		}
	};
    inline constexpr CsvManager(const CsvManager&) = delete;
    inline constexpr CsvManager(CsvManager&&) = delete;
    inline constexpr auto operator=(const CsvManager&) = delete;
    inline constexpr auto operator=(CsvManager&&) = delete;
	inline void Reload(const std::string_view& configured_csv_name) {
		std::unique_lock<std::shared_mutex> guard{ this->mutex };
		auto iter = this->csv_table.find(configured_csv_name);
		if (iter != this->csv_table.end()) {
			try {
				auto csv_uptr = std::make_unique<rapidcsv::Document>((std::filesystem::path(BASE_CSV_DIR_PATH)/(configured_csv_name)).string());
				iter->second.reset(std::move(csv_uptr));
			}
			catch (const std::iostream::failure& e) {
				spdlog::error("Try to reload file \"{}\" ,failed: {}", configured_csv_name, e.what());
				spdlog::error("Old data of \"{}\" not change", configured_csv_name);
			}
		}
		else {
			ExceptionWhenCsvNameNotConfigure(configured_csv_name);
		}

	}
	inline bool IsCsvNameConfigured(const std::string_view& configured_csv_name) const{
		std::shared_lock<std::shared_mutex> guard{ this->mutex };
		return this->csv_table.find(configured_csv_name) != this->csv_table.end();
	}
	inline const std::vector<std::string_view>& GetConfiguredCsvNames() const{
		std::shared_lock<std::shared_mutex> guard{ this->mutex };
		return configured_csv_names;
	}
	inline void ConstAccessCsv(const std::string_view &configured_csv_name, const std::function<void(const rapidcsv::Document&)>& access) const {
		std::shared_lock<std::shared_mutex> guard{ this->mutex };
		auto iter = this->csv_table.find(configured_csv_name);
		if (iter != this->csv_table.end()) {
			iter->second.ConstAccess(access);
		}
		else {
			ExceptionWhenCsvNameNotConfigure(configured_csv_name);
		}
	}
    //void NotConstAccess(const std::string_view& configured_path, const std::function<void()>& access);
};

using CsvManagerSingleton = Singleton<CsvManager>;