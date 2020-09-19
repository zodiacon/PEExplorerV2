#pragma once

struct IBufferManager {
	virtual uint32_t GetData(int64_t offset, uint8_t* buffer, uint32_t count) = 0;
	virtual bool Insert(int64_t offset, const uint8_t* data, uint32_t count) = 0;
	virtual bool Delete(int64_t offset, size_t count) = 0;
	virtual bool SetData(int64_t offset, const uint8_t* data, uint32_t count) = 0;
	virtual int64_t GetSize() const = 0;
	virtual uint8_t* GetRawData(int64_t offset) = 0;
	virtual bool IsReadOnly() const = 0;
	virtual ~IBufferManager() = default;

	static std::unique_ptr<IBufferManager> CreateInMemory(uint32_t initialSize = 0);
	static std::unique_ptr<IBufferManager> CreateFromProcess(DWORD pid, int64_t address, int64_t size, bool readWrite = false);
	static std::unique_ptr<IBufferManager> CreateFromFile(PCWSTR path, bool readWrite = false, bool inMemory = false);
};

