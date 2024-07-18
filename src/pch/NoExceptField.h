#pragma once
#include <string_view>
namespace NoExceptField {
	constexpr std::string_view default_no_exception_run_message{ "Find no catch exception." };
	template<typename ReturnType>
	inline ReturnType FieldWithReturn(std::function<ReturnType()>&& code, const ReturnType& fail_return, std::optional<const std::string_view> fail_message = std::nullopt) noexcept {
		try {
			return code();
		}
		catch (const std::exception& no_catch_exception) {
			if (!fail_message.has_value()) [[likely]] {
				logger::info(default_no_exception_run_message.data());
				}
			else {
				logger::info("{}", fail_message.value());
			}
			logger::info("Exception what:{}", no_catch_exception.what());
			return fail_return;
		}
	}
	inline void Field(std::function<void()>&& code, std::optional<const std::string_view> fail_message = std::nullopt) noexcept {
		try {
			code();
		}
		catch (const std::exception& no_catch_exception) {
			if ( !fail_message.has_value())[[likely]] {
				logger::info(default_no_exception_run_message.data());
				}
			else {
				logger::info("{}", fail_message.value());
			}
			logger::info("Exception what:{}", no_catch_exception.what());
		}
	}
}