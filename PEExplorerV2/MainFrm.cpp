// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "GenericListView.h"
#include "SummaryView.h"
#include "ExportsView.h"
#include "SectionsView.h"
#include "ImportsView.h"
#include "ImportsFrameView.h"
#include "DataDirectoriesView.h"
#include "ResourcesFrameView.h"
#include "HexView.h"
#include "HexControl/InMemoryBuffer.h"
#include "StructureView.h"
#include "ImageDosHeaderStruct.h"
#include "ImageFileHeaderStruct.h"
#include "ImageOptionalHeaderStruct.h"
#include "Capstone/capstone.h"
#include "AssemblyView.h"
#include "ManagedTypesView.h"
#include "TypeMembersView.h"
#include "CLRMetadataParser.h"

#pragma comment(lib, "capstone/capstone.lib")

const DWORD ListViewDefaultStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS;

int CMainFrame::m_TotalFrames = 0;

CMainFrame::CMainFrame() {
	m_TotalFrames++;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) {
	if (m_view.PreTranslateMessage(pMsg))
		return TRUE;

	return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle() {
	UIUpdateToolBar();
	UIUpdateStatusBar();
	return FALSE;
}

void CMainFrame::OnFinalMessage(HWND) {
	delete this;
}

void CMainFrame::InitTree() {
	m_tree.LockWindowUpdate();
	m_tree.DeleteAllItems();

	auto rootIcon = m_Parser->IsExecutable() ? 6 : 7;
	auto objectFile = m_Parser->IsObjectFile();

	auto root = InsertTreeItem(m_FileName, rootIcon, rootIcon, TreeNodeType::Root);
	InsertTreeItem(L"Summary", 0, 0, TreeNodeType::Summary, root);
	InsertTreeItem(L"Sections", 1, 1, TreeNodeType::Sections, root);
	if(!objectFile)
		InsertTreeItem(L"Directories", 2, 2, TreeNodeType::Directories, root);
	if(m_Parser->HasExports())
		InsertTreeItem(L"Exports", 3, 3, TreeNodeType::Exports, root);

	if(m_Parser->HasImports())
		InsertTreeItem(L"Imports", 4, 4, TreeNodeType::Imports, root);
	if (!objectFile)
		InsertTreeItem(L"Resources", 5, 5, TreeNodeType::Resources, root);
	if(m_Parser->IsManaged())
		InsertTreeItem(L".NET (CLR)", 11, 11, TreeNodeType::DotNet, root);
	auto headers = InsertTreeItem(L"Headers", 8, 8, TreeNodeType::Headers, root);
	if (!objectFile)
		InsertTreeItem(L"DOS_HEADER", 9, 9, TreeNodeType::ImageDosHeader, headers);
	InsertTreeItem(L"FILE_HEADER", 9, 9, TreeNodeType::ImageFileHeader, headers);
	if (!objectFile)
		InsertTreeItem(L"OPTIONAL_HEADER", 9, 9, TreeNodeType::ImageOptionalHeader, headers);
	headers.Expand(TVE_EXPAND);

	m_tree.Expand(root);
	m_tree.LockWindowUpdate(FALSE);

	CreateNewTab(TreeNodeType::Summary);
	UpdateUI();
}

void CMainFrame::UpdateUI() {
	bool fileOpen = m_Parser != nullptr;

	UIEnable(ID_VIEW_SUMMARY, fileOpen);
	UIEnable(ID_VIEW_SECTIONS, fileOpen);
	UIEnable(ID_VIEW_EXPORTS, fileOpen && m_Parser->HasExports());
	UIEnable(ID_VIEW_IMPORTS, fileOpen && m_Parser->HasImports());
	UIEnable(ID_VIEW_RESOURCES, fileOpen && !m_Parser->IsObjectFile());
	UIEnable(ID_VIEW_DIRECTORIES, fileOpen && !m_Parser->IsObjectFile());
	UIEnable(ID_VIEW_DOTNET, fileOpen && m_Parser->IsManaged());
	UIEnable(ID_WINDOW_CLOSE, m_view.GetPageCount() > 0);
	UIEnable(ID_WINDOW_CLOSEALL, m_view.GetPageCount() > 0);
}

void CMainFrame::CreateNewTab(TreeNodeType type) {
	IStructureProvider* structProvider = nullptr;

	switch (type) {
		case TreeNodeType::Summary:
		{
			auto view = new SummaryView(m_Parser.get());
			auto summary = new CGenericListView(view, true);
			summary->Create(m_view, nullptr, nullptr, ListViewDefaultStyle);
			m_view.AddPage(*summary, L"Summary", 0, (PVOID)type);
			break;
		}

		case TreeNodeType::Exports:
		{
			auto view = new ExportsView(m_Parser.get(), this);
			auto lv = new CGenericListView(view, true);
			lv->Create(m_view, nullptr, nullptr, ListViewDefaultStyle);
			m_view.AddPage(*lv, L"Exports", 3, (PVOID)type);
			break;
		}

		case TreeNodeType::Imports:
		{
			auto view = new CImportsFrameView(m_Parser.get(), this);
			view->Create(m_view, rcDefault, nullptr, WS_CHILD | WS_VISIBLE);
			m_view.AddPage(*view, L"Imports", 4, (PVOID)type);
			break;
		}

		case TreeNodeType::Sections:
		{
			auto view = new SectionsView(m_Parser.get(), this);
			auto lv = new CGenericListView(view, true);
			lv->Create(m_view, nullptr, nullptr, ListViewDefaultStyle);
			m_view.AddPage(*lv, L"Sections", 1, (PVOID)type);
			break;
		}

		case TreeNodeType::Directories:
		{
			auto view = new DataDirectoriesView(m_Parser.get(), this);
			auto lv = new CGenericListView(view, true);
			lv->Create(m_view, nullptr, nullptr, ListViewDefaultStyle | LVS_NOSORTHEADER);
			m_view.AddPage(*lv, L"Directories", 2, (PVOID)type);
			break;
		}

		case TreeNodeType::Resources:
		{
			auto view = new CResourcesFrameView(m_Parser.get());
			view->Create(m_view, nullptr, nullptr, WS_CHILD | WS_VISIBLE);
			m_view.AddPage(*view, L"Resources", 5, (PVOID)type);
			break;
		}

		case TreeNodeType::DotNet:
		{
			if (!m_Parser->IsCLRMetadataAvailable()) {
				AtlMessageBox(*this, L"CLR Metadata is not available (probably bad format)", IDR_MAINFRAME, MB_ICONERROR);
				break;
			}
			auto view = new ManagedTypesView(m_Parser.get(), this);
			auto lv = new CGenericListView(view);
			lv->Create(m_view, nullptr, nullptr, ListViewDefaultStyle);
			m_view.AddPage(*lv, L".NET (CLR)", 9, (PVOID)type);
			break;
		}

		case TreeNodeType::ImageDosHeader:
			structProvider = new ImageDosHeaderStruct(m_Parser.get());

		case TreeNodeType::ImageFileHeader:
			if (structProvider == nullptr)
				structProvider = new ImageFileHeaderStruct(m_Parser.get());

		case TreeNodeType::ImageOptionalHeader:
			if (structProvider == nullptr)
				structProvider = new ImageOptionalHeaderStruct(m_Parser.get());
		{
			auto view = new StructureView(m_Parser.get(), structProvider);
			auto lv = new CGenericListView(view, true);
			lv->Create(m_view, rcDefault, nullptr, ListViewDefaultStyle | LVS_NOSORTHEADER);
			m_view.AddPage(*lv, structProvider->GetName(), 7, (PVOID)type);
			break;
		}
	}
	UIEnable(ID_WINDOW_CLOSE, m_view.GetPageCount() > 0);
}

bool CMainFrame::SwitchToTab(TreeNodeType type) {
	for (int page = 0; page < m_view.GetPageCount(); page++)
		if ((DWORD_PTR)m_view.GetPageData(page) == (DWORD_PTR)type) {
			m_view.SetActivePage(page);
			return true;
		}

	// create a new page
	CreateNewTab(type);
	return false;
}

bool CMainFrame::DoFileOpen(PCWSTR path, bool newWindow) {
	auto file = std::make_unique<PEParser>(path);
	if (file->IsImportLib()) {
		AtlMessageBox(*this, L"Import Library detected. PE Explorer is unable to parse such files",
			IDR_MAINFRAME, MB_ICONWARNING);
		return false;
	}

	if (!file->IsValid()) {
		AtlMessageBox(*this, L"Error opening file.", IDR_MAINFRAME, MB_ICONERROR);
		return false;
	}

	if (!newWindow) {
		m_Parser = std::move(file);
		m_FilePath = path;

		m_FileName = m_FilePath.Mid(m_FilePath.ReverseFind(L'\\') + 1);
		CString title;
		title.LoadString(IDR_MAINFRAME);
		SetWindowText(title + L" (" + m_FilePath + L")");
		m_view.RemoveAllPages();

		InitTree();
	}
	else {
		auto frame = new CMainFrame();
		frame->CreateEx();
		frame->DoFileOpen(path);
		frame->ShowWindow(SW_SHOWDEFAULT);
	}
	AddToRecentFiles(path);
	return true;
}

void CMainFrame::AddRecentFiles(bool first) {
	if (m_RecentFiles.empty())
		return;

	CMenuHandle menu(m_CmdBar.GetMenu());
	CMenu popup;
	popup.CreatePopupMenu();
	int i = 0;
	for (auto& file : m_RecentFiles) {
		popup.AppendMenuW(MF_BYCOMMAND, ATL_IDS_MRU_FILE + i++, file);
	}
	menu.GetSubMenu(0).InsertMenu(5, MF_BYPOSITION, popup.Detach(), L"Recent Files");
	if (m_RecentFiles.size() == 1 || first)
		menu.GetSubMenu(0).InsertMenu(6, MF_BYPOSITION | MF_SEPARATOR, 0);
}

void CMainFrame::AddToRecentFiles(PCWSTR file) {
	auto empty = m_RecentFiles.empty();
	auto it = std::find(m_RecentFiles.begin(), m_RecentFiles.end(), file);
	if (it != m_RecentFiles.end()) {
		// file exists, move to top of list
		m_RecentFiles.erase(it);
		m_RecentFiles.insert(m_RecentFiles.begin(), file);
	}
	else {
		m_RecentFiles.insert(m_RecentFiles.begin(), file);
		if (m_RecentFiles.size() > 20)
			m_RecentFiles.pop_back();
	}

	if (!empty) {
		CMenuHandle menu(m_CmdBar.GetMenu());
		menu = menu.GetSubMenu(0);
		menu.DeleteMenu(5, MF_BYPOSITION);
	}
	AddRecentFiles();
}

bool CMainFrame::SaveSettings() {
	WCHAR path[MAX_PATH];
	if (!::SHGetSpecialFolderPath(nullptr, path, CSIDL_LOCAL_APPDATA, TRUE))
		return false;

	::wcscat_s(path, L"\\PEExplorerV2.ini");
	int i = 0;
	for (auto& file : m_RecentFiles) {
		::WritePrivateProfileString(L"RecentFiles", (L"File" + std::to_wstring(i++)).c_str(), file, path);
	}
	return true;
}

bool CMainFrame::LoadSettings() {
	WCHAR path[MAX_PATH];
	if (!::SHGetSpecialFolderPath(nullptr, path, CSIDL_LOCAL_APPDATA, FALSE))
		return false;

	::wcscat_s(path, L"\\PEExplorerV2.ini");
	WCHAR file[MAX_PATH];
	for (int i = 0; i < 10; i++) {
		if (!::GetPrivateProfileString(L"RecentFiles", (L"File" + std::to_wstring(i)).c_str(), L"", file, MAX_PATH, path))
			break;
		if (*file == L'\0')
			break;
		m_RecentFiles.push_back(file);
	}

	return true;
}

CString CMainFrame::GetFileNameToOpen() {
	CSimpleFileDialog dlg(TRUE, nullptr, nullptr, OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_EXPLORER,
		L"All PE Files\0*.exe;*.dll;*.sys;*.efi;*.ocx;*.lib;*.obj\0"
		L"Executables (*.exe)\0*.exe\0Dynamic Link Libraries (*.dll)\0*.dll\0"
		L"Library Files (*.lib, *.obj)\0*.lib;*.obj\0"
		L"All Files\0*.*\0", *this);
	return dlg.DoModal() == IDOK ? dlg.m_szFileName : L"";
}

LRESULT CMainFrame::OnWindowClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int nActivePage = m_view.GetActivePage();
	if (nActivePage != -1) {
		auto data = PtrToInt(m_view.GetPageData(nActivePage));
		m_view.RemovePage(nActivePage);
		auto it = m_TreeNodes.find(data);
		ATLASSERT(it != m_TreeNodes.end());
		if(data >= 0x10000)
			it->second.Delete();
		m_TreeNodes.erase(data);
	}
	else
		::MessageBeep((UINT)-1);
	UIEnable(ID_WINDOW_CLOSE, m_view.GetPageCount() > 0);

	return 0;
}

LRESULT CMainFrame::OnWindowCloseAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	m_view.RemoveAllPages();

	return 0;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	DragAcceptFiles();
	LoadSettings();
	CString title, copyright;
	title.LoadStringW(IDR_MAINFRAME);
	copyright.LoadStringW(IDS_COPYRIGHT);
	SetWindowText(title + L" " + copyright);

	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, nullptr, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	m_CmdBar.AttachMenu(GetMenu());
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
		{ ID_VIEW_DOTNET, IDI_COMPONENT },
		{ ID_OBJECT_VIEWDATA, IDI_VIEW },
		{ ID_WINDOW_NEW, IDI_NEWWINDOW },
		{ ID_IMPORTLIB_OPENINNEWWINDOW, IDI_NEWWINDOW },
		{ ID_FILE_CLOSE, IDI_CLOSE },
		{ ID_WINDOW_CLOSE, IDI_DELETE },
	};
	for (const auto& cmd : cmds)
		m_CmdBar.AddIcon(AtlLoadIcon(cmd.icon), cmd.id);

	CToolBarCtrl tb;
	auto hWndToolBar = tb.Create(m_hWnd, nullptr, nullptr, ATL_SIMPLE_TOOLBAR_PANE_STYLE, 0, ATL_IDW_TOOLBAR);
	CImageList tbImages;
	tbImages.Create(24, 24, ILC_COLOR32, 8, 4);
	tb.SetImageList(tbImages);

	struct {
		UINT id;
		int image;
		int style = BTNS_BUTTON;
	} buttons[] = {
		{ ID_FILE_OPEN, IDI_OPEN },
		{ 0 },
		{ ID_EDIT_COPY, IDI_COPY },
		{ 0 },
		{ ID_VIEW_SUMMARY, IDI_INFO, 0 },
		{ ID_VIEW_SECTIONS, IDI_SECTIONS, 0 },
		{ ID_VIEW_DIRECTORIES, IDI_DIRS, 0 },
		{ ID_VIEW_EXPORTS, IDI_EXPORTS, 0 },
		{ ID_VIEW_IMPORTS, IDI_IMPORTS, 0 },
		{ ID_VIEW_RESOURCES, IDI_RESOURCES, 0 },
		{ ID_VIEW_DOTNET, IDI_COMPONENT, 0 },
		{ 0 },
		{ ID_WINDOW_CLOSE, IDI_DELETE },
	};
	for (auto& b : buttons) {
		if (b.id == 0)
			tb.AddSeparator(0);
		else {
			int image = tbImages.AddIcon(AtlLoadIconImage(b.image, 0, 24, 24));
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
		TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE, IDC_TREE);
	m_view.Create(m_splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	CMenuHandle menuMain = m_CmdBar.GetMenu();
	m_view.SetWindowMenu(menuMain.GetSubMenu(4));

	int size = 24;
	CImageList images;
	images.Create(size, size, ILC_COLOR32, 10, 4);
	UINT icons[] = {
		IDI_INFO, IDI_SECTIONS, IDI_DIRS, IDI_EXPORTS, IDI_IMPORTS, IDI_RESOURCES,
		IDI_FILE_EXE, IDI_FILE_DLL, IDI_HEADERS, IDI_STRUCT, IDI_EXPORT, IDI_COMPONENT,
		IDI_CLASS
	};

	for (auto id : icons)
		images.AddIcon(AtlLoadIconImage(id, 64, size, size));
	m_tree.SetImageList(images.Detach(), TVSIL_NORMAL);

	m_splitter.SetSplitterPanes(m_tree, m_view);
	UpdateLayout();
	m_splitter.SetSplitterPos(250);

	CImageList tabImages;
	tabImages.Create(16, 16, ILC_COLOR32, 10, 4);
	UINT tabicons[] = {
		IDI_INFO, IDI_SECTIONS, IDI_DIRS, IDI_EXPORTS, IDI_IMPORTS, IDI_RESOURCES, IDI_HEADERS, IDI_STRUCT,
		IDI_EXPORT, IDI_COMPONENT, IDI_CLASS
	};
	for (auto id : tabicons)
		tabImages.AddIcon(AtlLoadIconImage(id, 64, 16, 16));
	m_view.SetImageList(tabImages.Detach());

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	AddRecentFiles(true);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != nullptr);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UpdateUI();
	UIEnable(ID_WINDOW_CLOSE, FALSE);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
	SaveSettings();

	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != nullptr);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	if (--m_TotalFrames == 0)
		bHandled = FALSE;
	else
		bHandled = TRUE;
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

LRESULT CMainFrame::OnFileOpen(WORD, WORD, HWND, BOOL&) {
	auto filename = GetFileNameToOpen();
	if (filename.IsEmpty())
		return 0;

	DoFileOpen(filename);
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
	SwitchToTab((TreeNodeType)item.GetData());

	return 0;
}

LRESULT CMainFrame::OnViewDataItem(WORD, WORD id, HWND, BOOL&) {
	SwitchToTab((TreeNodeType)(id - ID_VIEW_SUMMARY + 1));
	return 0;
}

LRESULT CMainFrame::OnDropFiles(UINT, WPARAM wParam, LPARAM, BOOL&) {
	auto hDrop = reinterpret_cast<HDROP>(wParam);
	auto count = ::DragQueryFile(hDrop, -1, nullptr, 0);
	if (count != 1)
		return 0;

	WCHAR path[MAX_PATH];
	if (::DragQueryFile(hDrop, 0, path, MAX_PATH)) {
		DoFileOpen(path);
	}
	::DragFinish(hDrop);

	return 0;
}

LRESULT CMainFrame::OnWindowActivate(WORD, WORD id, HWND, BOOL&) {
	int nPage = id - ID_WINDOW_TABFIRST;
	m_view.SetActivePage(nPage);
	return 0;
}

LRESULT CMainFrame::OnRecentFile(WORD, WORD id, HWND, BOOL&) {
	int index = id - ATL_IDS_MRU_FILE;
	auto path = m_RecentFiles[index];
	if (path == m_FilePath && m_Parser != nullptr)
		return 0;

	DoFileOpen(path);
	return 0;
}

LRESULT CMainFrame::OnOpenInNewWindow(WORD, WORD, HWND, BOOL&) {
	auto path = GetFileNameToOpen();
	if (path.IsEmpty())
		return 0;
	DoFileOpen(path, true);
	return 0;
}

LRESULT CMainFrame::OnAlwaysOnTop(WORD, WORD id, HWND, BOOL&) {
	bool onTop = GetExStyle() & WS_EX_TOPMOST;
	SetWindowPos(onTop ? HWND_NOTOPMOST : HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	UISetCheck(id, !onTop);
	return 0;
}

LRESULT CMainFrame::OnNewWindow(WORD, WORD, HWND, BOOL&) {
	auto frame = new CMainFrame();
	frame->CreateEx();
	frame->ShowWindow(SW_SHOWDEFAULT);

	return 0;
}

CTreeItem CMainFrame::CreateAssemblyView(const ExportedSymbol& sym) {
	CTreeItem node;
	csh handle;
	cs_insn* insn;
	if (cs_open(CS_ARCH_X86, m_Parser->IsPe64() ? CS_MODE_64 : CS_MODE_32, &handle) != CS_ERR_OK)
		return node;
	auto address = (const uint8_t*)m_Parser->GetAddress(sym.Address);
	auto count = cs_disasm(handle, address, 0x600, m_Parser->GetOptionalHeader64().ImageBase + sym.Address, 0, &insn);
	if (count > 0) {
		node = m_TreeNodes[int(TreeNodeType::Exports)].InsertAfter(CString(sym.Name.c_str()), nullptr, 10);
		auto view = new CAssemblyView(insn, static_cast<int>(count));
		view->Create(m_view, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		auto data = (int)TreeNodeType::ExportView + sym.Ordinal;
		m_view.AddPage(*view, CString(sym.Name.c_str()), 8, IntToPtr(data));
		node.EnsureVisible();
		node.SetData(data);
		m_TreeNodes.insert({ data, node });
	}

	cs_close(&handle);
	return node;
}

bool CMainFrame::OpenDocument(PCWSTR name, bool newWindow) {
	return DoFileOpen(name, newWindow);
}

CTreeItem CMainFrame::CreateTypeMembersView(const ManagedType& type) {
	size_t data = type.Token + (size_t)TreeNodeType::ManagedTypeMembersView;
	auto it = m_TreeNodes.find(data);
	if (it != m_TreeNodes.end()) {
		SwitchToTab((TreeNodeType)data);
		return it->second;
	}

	auto impl = new TypeMembersView(*m_Parser->GetCLRParser(), type);
	auto view = new CGenericListView(impl, true);
	auto hWnd = view->Create(m_view, rcDefault, nullptr, ListViewDefaultStyle);
	m_view.AddPage(hWnd, type.Name + L" Members", 10, (PVOID)data);
	auto node = m_TreeNodes[int(TreeNodeType::DotNet)].InsertAfter(type.Name, nullptr, 12);
	m_TreeNodes.insert({ data, node });
	node.EnsureVisible();
	node.SetData(data);

	return node;
}

CTreeItem CMainFrame::CreateHexView(TreeNodeType type, PCWSTR title, LPARAM param) {
	switch (type) {
		case TreeNodeType::SectionView:
		case TreeNodeType::DirectoryView:
		{
			bool sectionView = type == TreeNodeType::SectionView;
			auto number = (int)param;
			auto data = int(type) + number;
			auto it = m_TreeNodes.find(data);
			if (it != m_TreeNodes.end()) {
				ATLVERIFY(SwitchToTab((TreeNodeType)data));
				return it->second;
			}

			std::unique_ptr<IBufferManager> buffer = std::make_unique<InMemoryBuffer>(0);
			DWORD bias = 0;
			if (sectionView) {
				auto section = m_Parser->GetSectionHeader(number);
				auto start = section->VirtualAddress ? section->VirtualAddress : section->PointerToRawData;
				auto size = min(section->Misc.VirtualSize, section->SizeOfRawData);		
				buffer->SetData(0, (const BYTE*)m_Parser->GetAddress(start), size);
				bias = section->VirtualAddress;
			}
			else {
				ULONG size;
				auto address = m_Parser->GetDataDirectoryAddress(number, &size);
				ATLASSERT(address);
				buffer->SetData(0, (const BYTE*)address, size);
				bias = m_Parser->GetDataDirectory(number)->VirtualAddress;
			}
			int image = sectionView ? 1 : 2;
			auto node = m_TreeNodes[int(sectionView ? TreeNodeType::Sections : TreeNodeType::Directories)]
				.InsertAfter(title, nullptr, image);
			node.EnsureVisible();
			node.SetData(data);
			m_TreeNodes.insert({ data, node });
			auto view = new CHexView(std::move(buffer), node);
			view->Create(m_view, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
			view->GetHexControl().SetBiasOffset(bias);
			m_view.AddPage(*view, title, image, IntToPtr(data));
			return node;
		}

	}

	ATLASSERT(false);
	return CTreeItem();
}

UINT CMainFrame::ShowContextMenu(HMENU menu, const POINT& pt, DWORD flags) {
	return (UINT)m_CmdBar.TrackPopupMenu(menu, flags, pt.x, pt.y);
}

template<typename T>
inline CTreeItem CMainFrame::InsertTreeItem(PCWSTR text, int image, int selectedImage, T data, HTREEITEM hParent, HTREEITEM hAfter) {
	auto item = m_tree.InsertItem(text, image, selectedImage, hParent, hAfter);
	item.SetData(static_cast<DWORD_PTR>(data));
	m_TreeNodes.insert({ static_cast<int>(data), item });
	return item;
}

