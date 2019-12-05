#include "pch.h"
#include "DataDirectoriesView.h"
#include "PEStrings.h"
#include "resource.h"

DataDirectoriesView::DataDirectoriesView(PEParser* parser, IMainFrame* frame) 
	: _parser(parser), _frame(frame) {
}

void DataDirectoriesView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 150);
	lv.InsertColumn(1, L"Index", LVCFMT_RIGHT, 80);
	lv.InsertColumn(2, L"Address", LVCFMT_RIGHT, 120);
	lv.InsertColumn(3, L"Size", LVCFMT_RIGHT, 120);
}

int DataDirectoriesView::GetItemCount() {
	if (_dirs[0].Name.IsEmpty()) {
		PCWSTR names[] = {
			L"Export", L"Import", L"Resource", L"Exception", L"Security", L"Base Relocation",
			L"Debug", L"Architecture", L"Global Pointer", L"Thread Local Storage (TLS)", L"Load Config",
			L"Bound Import", L"Import Address Table (IAT)", L"Delay Import", L"COM Descriptor (CLR)"
		};

		ATLASSERT(_countof(names) == _countof(_dirs));

		for (int i = 0; i < _countof(_dirs); i++) {
			auto& dir = _dirs[i];
			dir.Name = names[i];
			auto dd = _parser->GetDataDirectory(i);
			dir.Address = dd->VirtualAddress;
			dir.Size = dd->Size;
			dir.Index = i;
		}
	}
	return _countof(_dirs);
}

CString DataDirectoriesView::GetItemText(int row, int col) {
	auto& dir = _dirs[row];
	switch (col) {
		case 0: return dir.Name;
		case 1: return std::to_wstring(dir.Index).c_str();
		case 2: return PEStrings::ToHex(dir.Address);
		case 3: return PEStrings::ToDecAndHex(dir.Size);
	}
	ATLASSERT(false);
	return L"";
}

bool DataDirectoriesView::Sort(int column, bool ascending) {
	return false;
}

void DataDirectoriesView::OnContextMenu(const POINT& pt, int index) {
	CMenuHandle menu;
	menu.LoadMenuW(IDR_CONTEXT);
	auto cmd = _frame->ShowContextMenu(menu.GetSubMenu(0), pt, TPM_RETURNCMD);
	if (cmd == ID_OBJECT_VIEWDATA) {
		_frame->CreateHexView(TreeNodeType::DirectoryView, _dirs[index].Name, index);
	}
}
