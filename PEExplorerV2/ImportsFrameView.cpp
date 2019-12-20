#include "pch.h"
#include "ImportsFrameView.h"

const DWORD ListViewDefaultStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

CImportsFrameView::CImportsFrameView(PEParser* parser)
	: m_Parser(parser), m_libViewImpl(parser), m_libView(&m_libViewImpl),
	m_importsView(&m_importsImpl) {
}

LRESULT CImportsFrameView::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
	m_splitter.Create(m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_libView.Create(m_splitter, rcDefault, nullptr, ListViewDefaultStyle | LVS_SINGLESEL, WS_EX_CLIENTEDGE, IDC_LIBS);

	m_importsView.Create(m_splitter, rcDefault, nullptr, ListViewDefaultStyle, WS_EX_CLIENTEDGE);

	m_splitter.SetSplitterPanes(m_libView, m_importsView);
	m_splitter.SetSplitterPosPct(33);

	return 0;
}

LRESULT CImportsFrameView::OnSize(UINT, WPARAM, LPARAM lParam, BOOL&) {
	m_splitter.MoveWindow(0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	return 0;
}

LRESULT CImportsFrameView::OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&) {
	return 1;
}

LRESULT CImportsFrameView::OnLibraryChanged(int, LPNMHDR, BOOL&) {
	int selected = m_libView.GetSelectedIndex();
	if (selected < 0) {
		m_importsImpl.SetSymbols(std::vector<ImportedSymbol>());
	}
	else {
		m_importsImpl.SetSymbols(m_libViewImpl.GetLibrary(selected).Symbols);
	}
	m_importsView.ClearSort(0);
	m_importsView.Refresh();

	return LRESULT();
}
