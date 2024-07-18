#pragma once
#include <CsvLoadLibApi.h>
class CsvLoadLibApiObj{
private:
	inline static CsvLoadLibApi* ptr = nullptr;
public:
	friend void CsvManagerMsgListener(SFSE::MessagingInterface::Message* message) noexcept;
	inline static const CsvLoadLibApi *get() {
		return ptr;
	}
};