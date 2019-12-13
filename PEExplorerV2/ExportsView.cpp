#include "pch.h"
#include "ExportsView.h"
#include "PEStrings.h"
#include <algorithm>
#include "SortHelper.h"
#include "resource.h"

ExportsView::ExportsView(PEParser* parser, IMainFrame* frame) 
	: _parser(parser), _frame(frame) {
}

void ExportsView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 240);
	lv.InsertColumn(1, L"Ordinal", LVCFMT_RIGHT, 80);
	lv.InsertColumn(2, L"Address", LVCFMT_LEFT, 200);
	lv.InsertColumn(3, L"Undecorated Name", LVCFMT_LEFT, 300);

	CImageList images;
	images.Create(16, 16, ILC_COLOR32 | ILC_COLOR, 2, 0);
	images.AddIcon(AtlLoadIconImage(IDI_EXPORT, 0, 16, 16));
	images.AddIcon(AtlLoadIconImage(IDI_FORWARD, 0, 16, 16));
	lv.SetImageList(images, LVSIL_SMALL);
}

int ExportsView::GetItemCount() {
	_exports = _parser->GetExports();

	return static_cast<int>(_exports.size());
}

CString ExportsView::GetItemText(int row, int col) {
	auto& item = _exports[row];
	switch (col) {
		case 0:
			return item.Name.c_str();
		case 1:
			return item.Ordinal == 0xffff ? L"" : std::to_wstring(item.Ordinal).c_str();
		case 2:
			return item.ForwardName.empty() ? PEStrings::ToHex(item.Address, true) : CString(item.ForwardName.c_str());
		case 3:
			return item.UndecoratedName.c_str();
	}
	return L"";
}

bool ExportsView::Sort(int column, bool ascending) {
	std::sort(_exports.begin(), _exports.end(), [column, ascending](auto& e1, auto& e2) {
		return CompareItems(e1, e2, column, ascending);
		});
	return true;
}

int ExportsView::GetIcon(int row) {
	return _exports[row].ForwardName.empty() ? 0 : 1;
}

void ExportsView::OnContextMenu(const POINT& pt, int selected) {
	CMenuHandle menu;
	menu.LoadMenuW(IDR_CONTEXT);
	auto cmd = _frame->ShowContextMenu(menu.GetSubMenu(1), pt, TPM_RETURNCMD);
	if (cmd == ID_OBJECT_VIEWDATA) {
		_frame->CreateAssemblyView(_exports[selected]);
	}
}

bool ExportsView::CompareItems(const ExportedSymbol& e1, const ExportedSymbol& e2, int col, bool asc) {
	switch (col) {
		case 0:	return SortHelper::SortStrings(e1.Name, e2.Name, asc);
		case 1: return SortHelper::SortNumbers(e1.Ordinal, e2.Ordinal, asc);
		case 2: return SortHelper::SortNumbers(e1.Address, e2.Address, asc);
		case 3: return SortHelper::SortStrings(e1.UndecoratedName, e2.UndecoratedName, asc);
	}
	ATLASSERT(false);
	return false;
}
