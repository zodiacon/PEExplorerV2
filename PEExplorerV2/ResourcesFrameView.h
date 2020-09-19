#pragma once

#include "ResourcesView.h"
#include "HexView.h"

class PEParser;

class CResourcesFrameView :
	public CWindowImpl<CResourcesFrameView> {
public:
	CResourcesFrameView(PEParser* parser);

	enum { IDC_RES = 125, IDC_HEX };

	BEGIN_MSG_MAP(CResourcesFrameView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		NOTIFY_HANDLER(IDC_RES, LVN_ITEMCHANGED, OnResourceChanged)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

private:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnResourceChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

private:
	CSplitterWindow m_splitter;
	CGenericListView m_resView;
	CHexView m_hexView;
	int m_SelectedIndex{ -1 };
	ResourcesView m_resViewImpl;
	std::unique_ptr<IBufferManager> m_buffer;
};
