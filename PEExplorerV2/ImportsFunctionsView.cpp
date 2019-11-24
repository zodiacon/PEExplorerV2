#include "pch.h"
#include "ImportsFunctionsView.h"
#include "resource.h"
#include <algorithm>
#include "SortHelper.h"

void ImportsFunctionsView::SetSymbols(const std::vector<ImportedSymbol>& symbols) {
	_symbols = symbols;
}

void ImportsFunctionsView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 250);
	lv.InsertColumn(1, L"Hint", LVCFMT_RIGHT, 80);
	lv.InsertColumn(2, L"Undecorated Name", LVCFMT_LEFT, 300);

	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 1, 0);
	images.AddIcon(AtlLoadIconImage(IDI_IMPORT, 0, 16, 16));

	lv.SetImageList(images.Detach(), LVSIL_SMALL);
}

int ImportsFunctionsView::GetItemCount() {
	return static_cast<int>(_symbols.size());
}

CString ImportsFunctionsView::GetItemText(int row, int col) {
	auto& item = _symbols[row];

	switch (col) {
		case 0: return CString(item.Name.c_str());
		case 1: return std::to_wstring(item.Hint).c_str();
		case 2: return CString(item.UndecoratedName.c_str());
	}
	ATLASSERT(false);
	return L"";
}

bool ImportsFunctionsView::Sort(int column, bool ascending) {
	std::sort(_symbols.begin(), _symbols.end(), [column, ascending](auto& s1, auto& s2) {
		return CompareItems(s1, s2, column, ascending);
		});
	return true;
}

int ImportsFunctionsView::GetIcon(int row) {
	return 0;
}

bool ImportsFunctionsView::CompareItems(const ImportedSymbol& s1, const ImportedSymbol& s2, int col, bool asc) {
	switch (col) {
		case 0: return SortHelper::SortStrings(s1.Name, s2.Name, asc);
		case 1: return SortHelper::SortNumbers(s1.Hint, s2.Hint, asc);
		case 2: return SortHelper::SortStrings(s1.UndecoratedName, s2.UndecoratedName, asc);
	}
	ATLASSERT(false);
	return false;
}
