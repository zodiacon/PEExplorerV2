#include "pch.h"
#include "ResourcesView.h"
#include "PEStrings.h"
#include <algorithm>
#include "SortHelper.h"

ResourcesView::ResourcesView(PEParser* parser) : _parser(parser) {
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
	std::sort(_items.begin(), _items.end(), [column, ascending](const auto& res1, const auto& res2) {
		return CompareItems(res1, res2, column, ascending);
		});

	return true;
}

const ResourceItem& ResourcesView::GetResource(int index) const {
	return _items[index];
}

bool ResourcesView::CompareItems(const ResourceItem& res1, const ResourceItem& res2, int col, bool asc) {
	switch (col) {
		case 0:
			return SortHelper::SortStrings(res1.Type, res2.Type, asc);

		case 1:
			if (res1.Resource.IsId != res2.Resource.IsId)
				return SortHelper::SortBoolean(res1.Resource.IsId, res2.Resource.IsId, asc);
			if (res1.Resource.IsId)
				return SortHelper::SortNumbers(res1.Resource.Id, res2.Resource.Id, asc);
			return SortHelper::SortStrings(res1.Resource.Name, res2.Resource.Name, asc);

		case 2: return SortHelper::SortNumbers(res1.Resource.Rva, res2.Resource.Rva, asc);
		case 3:	return SortHelper::SortNumbers(res1.Resource.Size, res2.Resource.Size, asc);
	}

	ATLASSERT(false);
	return false;
}
