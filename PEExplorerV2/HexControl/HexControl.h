#pragma once

#include "IHexControl.h"
#include <unordered_map>

class CommandManager;

struct Range {
	int64_t Offset, Size;
	bool IsInRange(int64_t offset) const;
};

class CHexControl : public CWindow,	public IHexControl {
public:
	CHexControl();
	~CHexControl();

	enum { IDC_HEX = 0x788 };

	HWND Create(HWND hParent, RECT& rc = rcDefault, DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, DWORD exstyle = 0, UINT id = IDC_HEX);

	enum {
		HEXCN_CONTEXTMENU = 1
	};

	enum class OffsetFlags : uint8_t {
		None = 0,
		Modified = 1
	};

	// Inherited via IHexControl
	void SetBufferManager(IBufferManager* mgr) override;
	void SetReadOnly(bool readonly) override;
	bool IsReadOnly() const override;
	void SetAllowExtension(bool allow) override;
	bool IsAllowExtension() const override;
	bool CanUndo() const override;
	bool CanRedo() const override;
	bool Undo() override;
	bool Redo() override;
	void SetSize(int64_t size) override;
	bool SetDataSize(int32_t size) override;
	int32_t GetDataSize() const override;
	bool SetBytesPerLine(int32_t bytesPerLine) override;
	int32_t GetBytesPerLine() const override;
	void SetOffsetColor(COLORREF color = CLR_INVALID);
	void SetDataColor(COLORREF color = CLR_INVALID);
	void SetAsciiColor(COLORREF color = CLR_INVALID);
	void SetBackColor(COLORREF color = CLR_INVALID);
	bool Copy(int64_t offset = -1, int64_t size = 0);
	bool CanCopy() const override;
	bool Paste(int64_t offset = -1);
	bool CanPaste() const override;
	COLORREF GetOffsetColor() const;
	COLORREF GetDataColor() const;
	COLORREF GetAsciiColor() const;
	COLORREF GetBackColor() const;
	bool Cut(int64_t offset = -1, int64_t size = 0) override;
	bool Delete(int64_t offset = -1, int64_t size = 0) override;
	bool CanCut() const override;
	bool CanDelete() const override;
	bool IsSelected(int64_t offset) const;
	int64_t SetBiasOffset(int64_t offset) override;
	int64_t GetBiasOffset() const override;
	CString GetText(int64_t offset, int64_t size) override;
	IBufferManager* GetBufferManager() const override;

private:
	struct Impl;
	std::unique_ptr<Impl> _impl;

};

DEFINE_ENUM_FLAG_OPERATORS(CHexControl::OffsetFlags);

