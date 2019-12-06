#pragma once

#include "HexControl/IHexControl.h"
#include "HexControl/HexControl.h"
#include "resource.h"

class CHexView : public CWindowImpl<CHexView> {
public:
	DECLARE_WND_CLASS_EX(nullptr, 0, COLOR_WINDOW);

	CHexView(std::unique_ptr<IBufferManager> buffer, CTreeItem treeItem);
	CHexView() {}

	IHexControl& GetHexControl();

	enum {
		Id_1Byte = 1000, Id_2Byte, Id_4Byte, Id_8Byte,
	};

	BEGIN_MSG_MAP(CHexView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_RANGE_HANDLER(Id_1Byte, Id_8Byte, OnChangeDataSize)
		COMMAND_ID_HANDLER(ID_FILE_EXPORT, OnExport)
	END_MSG_MAP()

private:
	bool DoExport(PCWSTR filename) const;

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnChangeDataSize(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnExport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CHexControl m_hex;
	CToolBarCtrl m_toolbar;
	std::unique_ptr<IBufferManager> m_buffer;
	CTreeItem m_treeItem;
};

