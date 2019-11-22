// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "aboutdlg.h"
#include "View.h"
#include "MainFrm.h"
#include "GenericListView.h"

const DWORD ListViewDefaultStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS;

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) {
	if (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle() {
	UIUpdateToolBar();
	return FALSE;
}

void CMainFrame::InitTree() {
	m_tree.LockWindowUpdate();
	m_tree.DeleteAllItems();
	
	auto rootIcon = m_Parser->IsExecutable() ? 6 : 7;
	auto root = InsertTreeItem(m_FileName, rootIcon, rootIcon, TreeNodeType::Root);
	InsertTreeItem(L"Summary", 0, 0, TreeNodeType::Summary, root);
	InsertTreeItem(L"Sections", 1, 1, TreeNodeType::Sections, root);
	InsertTreeItem(L"Directories", 2, 2, TreeNodeType::Directories, root);
	InsertTreeItem(L"Exports", 3, 3, TreeNodeType::Exports, root);
	InsertTreeItem(L"Imports", 4, 4, TreeNodeType::Imports, root);
	InsertTreeItem(L"Resources", 5, 5, TreeNodeType::Resources, root);

	m_tree.Expand(root);
	m_tree.LockWindowUpdate(FALSE);

	CreateNewTab(TreeNodeType::Summary);
	UpdateUI();
}

void CMainFrame::UpdateUI() {
	bool fileOpen = m_Parser != nullptr;

	UIEnable(ID_VIEW_SUMMARY, fileOpen);
	UIEnable(ID_VIEW_SECTIONS, fileOpen);
	UIEnable(ID_VIEW_EXPORTS, fileOpen);
	UIEnable(ID_VIEW_IMPORTS, fileOpen);
	UIEnable(ID_VIEW_RESOURCES, fileOpen);
	UIEnable(ID_VIEW_DIRECTORIES, fileOpen);
}

void CMainFrame::CreateNewTab(TreeNodeType type) {
	switch (type) {
		case TreeNodeType::Summary:
		{
			auto view = new SummaryView(m_Parser.get());
			auto summary = new CGenericListView(view, true);
			summary->Create(m_view, nullptr, nullptr, ListViewDefaultStyle);
			view->Init(*summary);
			m_view.AddPage(*summary, L"Summary", 0, (PVOID)type);
			break;
		}
	}
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, nullptr, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// remove old menu
	SetMenu(nullptr);
	m_CmdBar.m_bAlphaImages = true;

	struct {
		UINT id, icon;
	} cmds[] = {
		{ ID_EDIT_COPY, IDI_COPY },
		{ ID_FILE_OPEN, IDI_OPEN },
		{ ID_VIEW_SUMMARY, IDI_INFO },
		{ ID_VIEW_EXPORTS, IDI_EXPORTS },
		{ ID_VIEW_IMPORTS, IDI_IMPORTS },
		{ ID_VIEW_RESOURCES, IDI_RESOURCES },
		{ ID_VIEW_SECTIONS, IDI_SECTIONS },
		{ ID_VIEW_DIRECTORIES, IDI_DIRS },
	};
	for (auto& cmd : cmds)
		m_CmdBar.AddIcon(AtlLoadIcon(cmd.icon), cmd.id);

	CToolBarCtrl tb;
	auto hWndToolBar = tb.Create(m_hWnd, nullptr, nullptr, ATL_SIMPLE_TOOLBAR_PANE_STYLE, 0, ATL_IDW_TOOLBAR);
	CImageList tbImages;
	tbImages.Create(24, 24, ILC_COLOR32 | ILC_COLOR, 8, 4);
	tb.SetImageList(tbImages);

	struct {
		UINT id;
		int image;
		int style = BTNS_BUTTON;
	} buttons[] = {
		{ ID_FILE_OPEN, IDI_OPEN },
		{ 0 },
		{ ID_VIEW_SUMMARY, IDI_INFO, 0 },
		{ ID_VIEW_SECTIONS, IDI_SECTIONS, 0 },
		{ ID_VIEW_DIRECTORIES, IDI_DIRS, 0 },
		{ ID_VIEW_EXPORTS, IDI_EXPORTS, 0 },
		{ ID_VIEW_IMPORTS, IDI_IMPORTS, 0 },
		{ ID_VIEW_RESOURCES, IDI_RESOURCES, 0 },
	};
	for (auto& b : buttons) {
		if (b.id == 0)
			tb.AddSeparator(0);
		else {
			int image = tbImages.AddIcon(AtlLoadIcon(b.image));
			tb.AddButton(b.id, b.style, TBSTATE_ENABLED, image, nullptr, 0);
		}
	}

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, nullptr, TRUE);

	CReBarCtrl(m_hWndToolBar).LockBands(TRUE);

	CreateSimpleStatusBar();

	m_hWndClient = m_splitter.Create(m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	m_tree.Create(m_splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_HASLINES | 
		TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
	m_view.Create(m_splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	CMenuHandle menuMain = m_CmdBar.GetMenu();
	m_view.SetWindowMenu(menuMain.GetSubMenu(4));

	int size = 24;
	CImageList images;
	images.Create(size, size, ILC_COLOR32 | ILC_COLOR, 8, 4);
	UINT icons[] = {
		IDI_INFO, IDI_SECTIONS, IDI_DIRS, IDI_EXPORTS, IDI_IMPORTS, IDI_RESOURCES,
		IDI_FILE_EXE, IDI_FILE_DLL
	};

	for (auto id : icons)
		images.AddIcon(AtlLoadIconImage(id, 64, size, size));
	m_tree.SetImageList(images.Detach(), TVSIL_NORMAL);

	m_splitter.SetSplitterPanes(m_tree, m_view);
	UpdateLayout();
	m_splitter.SetSplitterPos(250);

	CImageList tabImages;
	tabImages.Create(16, 16, ILC_COLOR32 | ILC_COLOR, 6, 4);
	UINT tabicons[] = {
		IDI_INFO, IDI_SECTIONS, IDI_DIRS, IDI_EXPORTS, IDI_IMPORTS, IDI_RESOURCES
	};
	for (auto id : tabicons)
		tabImages.AddIcon(AtlLoadIconImage(id, 64, 16, 16));
	m_view.SetImageList(tabImages.Detach());

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != nullptr);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UpdateUI();

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != nullptr);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	CAboutDlg().DoModal();
	return 0;
}

LRESULT CMainFrame::OnViewTreePane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	bool bShow = (m_splitter.GetSinglePaneMode() != SPLIT_PANE_NONE);
	m_splitter.SetSinglePaneMode(bShow ? SPLIT_PANE_NONE : SPLIT_PANE_RIGHT);
	UISetCheck(ID_VIEW_TREEPANE, bShow);

	return 0;
}

LRESULT CMainFrame::OnFileOpen(WORD, WORD, HWND, BOOL&) {
	CSimpleFileDialog dlg(TRUE, nullptr, nullptr, OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_EXPLORER,
		L"Image Files (*.exe, *.dll, *.sys, *.ocx, *.efi)\0*.exe;*.dll;*.sys;*.efi;*.ocx\0All Files\0*.*\0", *this);
	if (dlg.DoModal() == IDOK) {
		auto file = std::make_unique<PEParser>(CStringA(dlg.m_szFileName));
		if (!file->IsValid()) {
			MessageBox(L"Error opening file.", L"PE Explorer");
			return 0;
		}

		m_Parser = std::move(file);
		m_FilePath = dlg.m_szFileName;
		m_FileName = dlg.m_szFileTitle;
		CString title;
		title.LoadString(IDR_MAINFRAME);
		SetWindowText(title + L" (" + m_FilePath + L")");
		m_view.RemoveAllPages();

		InitTree();
	}
	return 0;
}

LRESULT CMainFrame::OnFileClose(WORD, WORD, HWND, BOOL&) {
	m_tree.DeleteAllItems();
	m_view.RemoveAllPages();
	m_Parser.reset();
	CString title;
	title.LoadString(IDR_MAINFRAME);
	SetWindowText(title);
	UpdateUI();

	return 0;
}

LRESULT CMainFrame::OnTreeItemDoubleClick(int, LPNMHDR hdr, BOOL&) {
	auto item = m_tree.GetSelectedItem();
	// switch to the existing tab or create a new one
	for (int page = 0; page < m_view.GetPageCount(); page++)
		if ((DWORD_PTR)m_view.GetPageData(page) == item.GetData()) {
			m_view.SetActivePage(page);
			return 0;
		}

	// create a new page
	CreateNewTab((TreeNodeType)item.GetData());

	return 0;
}

template<typename T>
inline CTreeItem CMainFrame::InsertTreeItem(PCWSTR text, int image, int selectedImage, T data, HTREEITEM hParent, HTREEITEM hAfter) {
	auto item = m_tree.InsertItem(text, image, selectedImage, hParent, hAfter);
	item.SetData(static_cast<DWORD_PTR>(data));
	return item;
}

