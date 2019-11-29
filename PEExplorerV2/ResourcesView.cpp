#include "pch.h"
#include "ResourcesView.h"
#include "PEStrings.h"

ResourcesView::ResourcesView(PEParser* parser, CTreeItem resTreeItem) : _parser(parser) {
}

void ResourcesView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Type", LVCFMT_LEFT, 140);
	lv.InsertColumn(1, L"Name", LVCFMT_LEFT, 150);
	lv.InsertColumn(2, L"Address", LVCFMT_RIGHT, 100);
	lv.InsertColumn(3, L"Size", LVCFMT_RIGHT, 130);
}

int ResourcesView::GetItemCount() {
	if (_items.empty()) {
		for (auto& type : _parser->EnumResources()) {
			for (auto& res : type.Items) {
				ResourceItem item;
				CString typeName;
				if (type.IsId)
					typeName = PEStrings::ResourceTypeToString(type.Id);
				item.Type = typeName.IsEmpty() ? type.Name : typeName;
				item.Resource = res;
				_items.push_back(std::move(item));
			}
		}
	}

	return static_cast<int>(_items.size());
}

CString ResourcesView::GetItemText(int row, int col) {
	auto& res = _items[row];

	switch (col) {
		case 0: return res.Type;
		case 1: return res.Resource.Name;
		case 2: return PEStrings::ToHex(res.Resource.Rva);
		case 3: return PEStrings::ToDecAndHex(res.Resource.Size);
	}
	ATLASSERT(false);
	return L"";
}

bool ResourcesView::Sort(int column, bool ascending) {
	return false;
}
