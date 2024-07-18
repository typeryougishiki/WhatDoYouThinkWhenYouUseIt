#pragma once
#include <functional>
#include <string_view>
#include "rapidcsv.h"


struct CsvLoadLibApi {
	static constexpr std::string_view MESSAGER_SENDER{"CsvLoadLib"};
	std::function<void(const std::string_view&)> Reload ;
	std::function<bool(const std::string_view&)>IsCsvNameConfigured;
	std::function<const std::vector<std::string_view>& ()>GetConfiguredCsvNames;
	std::function<void(const std::string_view&, const std::function<void(const rapidcsv::Document&)>&)>ConstAccessCsv;
};
