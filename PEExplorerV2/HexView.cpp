#include "pch.h"
#include "HexView.h"
#include "resource.h"
#include "HexControl/IBufferManager.h"

CHexView::CHexView(std::unique_ptr<IBufferManager> buffer, CTreeItem treeItem) 
	: m_buffer(std::move(buffer)), m_treeItem(treeItem) {
}

IHexControl& CHexView::GetHexControl() {
	return m_hex;
}

bool CHexView::DoExport(PCWSTR filename) const {
	auto hFile = ::CreateFile(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	auto buffer = m_hex.GetBufferManager();
	DWORD written;
	if (!::WriteFile(hFile, buffer->GetRawData(0), (DWORD)buffer->GetSize(), &written, nullptr))
		return false;

	::CloseHandle(hFile);
	return true;
}

LRESULT CHexView::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
	m_hex.Create(*this, rcDefault);
	m_hex.SetBufferManager(m_buffer.get());
	m_hex.SetReadOnly(true);

	m_toolbar.Create(*this, rcDefault, nullptr, ATL_SIMPLE_TOOLBAR_STYLE | WS_BORDER | TBSTYLE_FLAT, WS_EX_CLIENTEDGE);
	CImageList tbImages;
	tbImages.Create(24, 24, ILC_COLOR32 | ILC_COLOR, 8, 4);
	m_toolbar.SetImageList(tbImages);

	struct {
		UINT id;
		int image;
		int style = BTNS_BUTTON;
		int state = TBSTATE_ENABLED;
	} buttons[] = {
		{ Id_1Byte, IDI_NUM1, BTNS_CHECKGROUP, TBSTATE_CHECKED },
		{ Id_2Byte, IDI_NUM2, BTNS_CHECKGROUP },
		{ Id_4Byte, IDI_NUM4, BTNS_CHECKGROUP },
		{ Id_8Byte, IDI_NUM8, BTNS_CHECKGROUP },
		{ 0 },
		{ ID_FILE_EXPORT, IDI_SAVE }
	};
	for (auto& b : buttons) {
		if (b.id == 0)
			m_toolbar.AddSeparator(0);
		else {
			int image = tbImages.AddIcon(AtlLoadIconImage(b.image, 64, 24, 24));
			m_toolbar.AddButton(b.id, b.style, TBSTATE_ENABLED | b.state, image, nullptr, 0);
		}
	}

	return 0;
}

LRESULT CHexView::OnDestroy(UINT, WPARAM, LPARAM, BOOL&) {
	if(m_treeItem)
		m_treeItem.Delete();

	return 0;
}

LRESULT CHexView::OnSize(UINT, WPARAM, LPARAM lParam, BOOL&) {
	if (m_hex) {
		CRect rc;
		m_toolbar.GetClientRect(&rc);
		m_toolbar.MoveWindow(0, 0, rc.Width(), rc.Height());
		m_hex.MoveWindow(0, rc.bottom, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) - rc.bottom);
	}
	return 0;
}

LRESULT CHexView::OnChangeDataSize(WORD, WORD id, HWND, BOOL&) {
	int size = (1 << (id - Id_1Byte));
	m_hex.SetDataSize(size);

	return 0;
}

LRESULT CHexView::OnExport(WORD, WORD, HWND, BOOL&) {
	CSimpleFileDialog dlg(FALSE, nullptr, nullptr, OFN_EXPLORER | OFN_OVERWRITEPROMPT, L"All Files\0*.*\0", *this);
	if (dlg.DoModal() == IDOK) {
		DoExport(dlg.m_szFileName);
	}
	return 0;
}
