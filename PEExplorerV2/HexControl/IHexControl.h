#pragma once

struct IBufferManager;

struct HexControlNotifyData {
	NMHDR hdr;
	POINT pt;
};

struct IHexControl {
	virtual void SetBufferManager(IBufferManager* mgr) = 0;
	virtual void SetReadOnly(bool readonly) = 0;
	virtual bool IsReadOnly() const = 0;
	virtual void SetAllowExtension(bool allow) = 0;
	virtual bool IsAllowExtension() const = 0;
	virtual bool CanUndo() const = 0;
	virtual bool CanRedo() const = 0;
	virtual bool Undo() = 0;
	virtual bool Redo() = 0;
	virtual void SetSize(int64_t size) = 0;
	virtual bool SetDataSize(int32_t size) = 0;
	virtual int32_t GetDataSize() const = 0;
	virtual bool SetBytesPerLine(int32_t bytesPerLine) = 0;
	virtual int32_t GetBytesPerLine() const = 0;
	virtual bool Copy(int64_t offset = -1, int64_t size = 0) = 0;
	virtual bool Paste(int64_t offset = -1) = 0;
	virtual bool CanCopy() const = 0;
	virtual bool CanPaste() const = 0;
	virtual bool Cut(int64_t offset = -1, int64_t size = 0) = 0;
	virtual bool Delete(int64_t offset = -1, int64_t size = 0) = 0;
	virtual bool CanCut() const = 0;
	virtual bool CanDelete() const = 0;
	virtual int64_t SetBiasOffset(int64_t offset) = 0;
	virtual int64_t GetBiasOffset() const = 0;
	virtual CString GetText(int64_t offset, int64_t size) = 0;
	virtual IBufferManager* GetBufferManager() const = 0;
};
