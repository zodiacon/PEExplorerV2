#include "pch.h"
#include "ImportsView.h"
#include "resource.h"
#include <algorithm>
#include "SortHelper.h"

ImportsView::ImportsView(PEParser* parser) : _parser(parser) {
}

void ImportsView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Library Name", LVCFMT_LEFT, 250);
	lv.InsertColumn(1, L"Imports Count", LVCFMT_RIGHT, 100);

	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 2, 0);
	images.AddIcon(AtlLoadIconImage(IDI_LIBRARY_IMPORT, 0, 16, 16));
	images.AddIcon(AtlLoadIconImage(IDI_APISET_LIB, 0, 16, 16));

	lv.SetImageList(images.Detach(), LVSIL_SMALL);
}

int ImportsView::GetItemCount() {
	if (_libraries.empty()) {
		_libraries = _parser->GetImports();
	}
	return static_cast<int>(_libraries.size());
}

CString ImportsView::GetItemText(int row, int col) {
	auto& item = _libraries[row];

	switch (col) {
		case 0: return CString(item.Name.c_str());
		case 1: return std::to_wstring(item.Symbols.size()).c_str();
	}
	ATLASSERT(false);
	return L"";
}

bool ImportsView::Sort(int column, bool ascending) {
	std::sort(_libraries.begin(), _libraries.end(), [ascending, column](auto& lib1, auto& lib2) {
		return CompareItems(lib1, lib2, column, ascending);
		});
	return true;
}

int ImportsView::GetIcon(int row) {
	auto prefix = _libraries[row].Name.substr(0, 7);
	return prefix == "api-ms-" || prefix == "ext-ms-" ? 1 : 0;
}

const ImportedLibrary& ImportsView::GetLibrary(int selected) const {
	return _libraries[selected];
}

bool ImportsView::CompareItems(ImportedLibrary& lib1, ImportedLibrary& lib2, int col, bool asc) {
	switch (col) {
		case 0: return SortHelper::SortStrings(lib1.Name, lib2.Name, asc);
		case 1: return SortHelper::SortNumbers(lib1.Symbols.size(), lib2.Symbols.size(), asc);
	}
	ATLASSERT(false);
	return false;
}
