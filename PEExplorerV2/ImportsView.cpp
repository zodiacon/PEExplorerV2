#include "pch.h"
#include "ImportsView.h"

ImportsView::ImportsView(PEParser* parser) : _parser(parser) {
}

void ImportsView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Library Name", LVCFMT_LEFT, 250);
	lv.InsertColumn(1, L"Imports Count", LVCFMT_RIGHT, 100);
}

int ImportsView::GetItemCount() {
	if (_libraries.empty()) {
		auto imports = _parser->GetImports();
		for (auto& lib : imports) {
			ImportLibrary library;
			library.Name = CString(lib.Name.c_str());
			library.Count = static_cast<int>(lib.Symbols.size());
			_libraries.push_back(library);
		}
	}
	return static_cast<int>(_libraries.size());
}

CString ImportsView::GetItemText(int row, int col) {
	auto& item = _libraries[row];

	switch (col) {
		case 0: return item.Name;
		case 1: return std::to_wstring(item.Count).c_str();
	}
	ATLASSERT(false);
	return L"";
}

bool ImportsView::Sort(int column, bool ascending) {
	return false;
}
