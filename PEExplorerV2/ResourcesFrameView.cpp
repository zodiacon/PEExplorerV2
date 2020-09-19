#include "pch.h"
#include "ResourcesFrameView.h"
#include "HexControl/InMemoryBuffer.h"

#ifdef _DEBUG
#pragma comment(lib, "HexControl/HexControld.lib")
#else
#pragma comment(lib, "HexControl/HexControl.lib")
#endif

CResourcesFrameView::CResourcesFrameView(PEParser* parser) : m_resViewImpl(parser), m_resView(&m_resViewImpl) {
}

LRESULT CResourcesFrameView::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
	const DWORD ListViewDefaultStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	m_splitter.Create(m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_resView.Create(m_splitter, rcDefault, nullptr, ListViewDefaultStyle | LVS_SINGLESEL, WS_EX_CLIENTEDGE, IDC_RES);

	m_hexView.Create(m_splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, IDC_HEX);
	auto& hex = m_hexView.GetHexControl();
	hex.SetBytesPerLine(16);

	m_splitter.SetSplitterPanes(m_resView, m_hexView);
	m_splitter.SetSplitterPosPct(40);

	return 0;
}

LRESULT CResourcesFrameView::OnSize(UINT, WPARAM, LPARAM lParam, BOOL&) {
	m_splitter.MoveWindow(0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	return 0;
}

LRESULT CResourcesFrameView::OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&) {
	return 1;		// no erase
}

LRESULT CResourcesFrameView::OnResourceChanged(int, LPNMHDR, BOOL&) {
	auto index = m_resView.GetSelectedIndex();
	if (m_SelectedIndex == index)
		return 0;

	auto& hex = m_hexView.GetHexControl();
	if (index < 0) {
		hex.SetBufferManager(nullptr);
	}
	else {
		auto res = m_resViewImpl.GetResource(index);
		m_buffer = std::make_unique<InMemoryBuffer>();
		m_buffer->SetData(0, (const BYTE*)res.Resource.Address, res.Resource.Size);
		hex.SetBufferManager(m_buffer.get());
	}
	m_SelectedIndex = index;

	return 0;
}
