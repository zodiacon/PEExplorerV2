#pragma once

#include "ImportsView.h"
#include "GenericListView.h"
#include "ImportsFunctionsView.h"

class CImportsFrameView : 
	public CWindowImpl<CImportsFrameView> {
public:
	CImportsFrameView(PEParser* parser);

	enum { IDC_LIBS = 123, IDC_IMPORTS };

	BEGIN_MSG_MAP(CImportsFrameView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		NOTIFY_HANDLER(IDC_LIBS, LVN_ITEMCHANGED, OnLibraryChanged)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

private:

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLibraryChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

private:
	CSplitterWindow m_splitter;
	CGenericListView m_libView;
	CGenericListView m_importsView;
	PEParser* m_Parser;
	ImportsView m_libViewImpl;
	ImportsFunctionsView m_importsImpl;
};

