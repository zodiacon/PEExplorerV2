#include "pch.h"
#include "ResourcesView.h"

ResourcesView::ResourcesView(PEParser* parser, CTreeItem resTreeItem) : _parser(parser) {
}

void ResourcesView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Type", LVCFMT_LEFT, 140);
	lv.InsertColumn(1, L"Name", LVCFMT_LEFT, 150);
	lv.InsertColumn(2, L"Address", LVCFMT_RIGHT, 120);
	lv.InsertColumn(3, L"Size", LVCFMT_RIGHT, 100);
}

int ResourcesView::GetItemCount() {
	if (_items.empty()) {
		for (auto& type : _parser->EnumResources()) {
			for (auto& name : type.Items) {
				ResourceItem item;
				item.Type = type.Name;
				item.Name = name;
				_parser->GetResourceInfo(type.Name, name, item.Address, item.Size);
				_items.push_back(std::move(item));
			}
		}
	}

	return static_cast<int>(_items.size());
}

CString ResourcesView::GetItemText(int row, int col) {
	return CString();
}

bool ResourcesView::Sort(int column, bool ascending) {
	return false;
}
