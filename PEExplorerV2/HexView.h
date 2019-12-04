#pragma once

#include "HexControl/IHexControl.h"
#include "HexControl/HexControl.h"

class CHexView : public CWindowImpl<CHexView> {
public:
	CHexView(std::unique_ptr<IBufferManager> buffer, CTreeItem treeItem);

	IHexControl& GetHexControl();

	BEGIN_MSG_MAP(CHexView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

private:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

private:
	CHexControl m_hex;
	std::unique_ptr<IBufferManager> m_buffer;
	CTreeItem m_treeItem;
};

