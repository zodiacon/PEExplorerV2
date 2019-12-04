#include "pch.h"
#include "HexView.h"

CHexView::CHexView(std::unique_ptr<IBufferManager> buffer, CTreeItem treeItem) 
	: m_buffer(std::move(buffer)), m_treeItem(treeItem) {
}

IHexControl& CHexView::GetHexControl() {
	return m_hex;
}

LRESULT CHexView::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
	m_hex.Create(*this, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	m_hex.SetBufferManager(m_buffer.get());
	m_hex.SetReadOnly(true);

	return 0;
}

LRESULT CHexView::OnDestroy(UINT, WPARAM, LPARAM, BOOL&) {
	m_treeItem.Delete();

	return 0;
}

LRESULT CHexView::OnSize(UINT, WPARAM, LPARAM lParam, BOOL&) {
	if (m_hex) {
		m_hex.MoveWindow(0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	return 0;
}

LRESULT CHexView::OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&) {
	return 1;
}
