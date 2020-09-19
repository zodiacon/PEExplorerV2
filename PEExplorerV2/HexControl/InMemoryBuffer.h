#pragma once

#include "IBufferManager.h"

class InMemoryBuffer : public IBufferManager {
public:
	InMemoryBuffer(uint32_t initialSize = 0);

	// Inherited via IBufferManager
	uint32_t GetData(int64_t offset, uint8_t * buffer, uint32_t count) override;
	bool Insert(int64_t offset, const uint8_t * data, uint32_t count) override;
	bool Delete(int64_t offset, size_t count) override;
	bool SetData(int64_t offset, const uint8_t * data, uint32_t count) override;
	int64_t GetSize() const override;
	uint8_t* GetRawData(int64_t offset) override;
	bool IsReadOnly() const override {
		return false;
	}

private:
	std::vector<uint8_t> _buffer;
};

