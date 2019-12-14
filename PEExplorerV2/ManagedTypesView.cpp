#include "pch.h"
#include "ManagedTypesView.h"
#include "CLRMetadataParser.h"
#include "PEStrings.h"
#include "resource.h"
#include <algorithm>
#include "SortHelper.h"

ManagedTypesView::ManagedTypesView(PEParser* parser) : _parser(parser) {
}

void ManagedTypesView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 350);
	lv.InsertColumn(1, L"Kind", LVCFMT_LEFT, 80);
	lv.InsertColumn(2, L"Token", LVCFMT_RIGHT, 100);
	lv.InsertColumn(3, L"Base Type", LVCFMT_LEFT, 200);
	lv.InsertColumn(4, L"Attributes", LVCFMT_RIGHT, 80);
	lv.InsertColumn(5, L"Details", LVCFMT_LEFT, 200);

	UINT icons[] = {
		IDI_ATTRIBUTE, IDI_CLASS, IDI_DELEGATE, IDI_ENUM, IDI_INTERFACE, IDI_STRUCT
	};
	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 6, 5);
	for (auto id : icons)
		images.AddIcon(AtlLoadIcon(id));
	lv.SetImageList(images, LVSIL_SMALL);
}

int ManagedTypesView::GetItemCount() {
	if (_types.empty()) {
		auto mp = _parser->GetCLRParser();
		ATLASSERT(mp);
		_types = mp->EnumTypes();
	}
	return static_cast<int>(_types.size());
}

CString ManagedTypesView::GetItemText(int row, int col) {
	auto& type = _types[row];
	switch (col) {
		case 0: return type.Name;
		case 1: return GetTypeKind(type.Kind);
		case 2: return PEStrings::ToHex((DWORD)type.Token);
		case 3: return type.BaseTypeName + L" (" + PEStrings::ToHex((DWORD)type.BaseTypeToken) + L")";
		case 4: return PEStrings::ToHex((DWORD)type.Attributes);
		case 5: return PEStrings::ManagedTypeAttributesToString(type.Attributes);
	}
	return L"";
}

bool ManagedTypesView::Sort(int column, bool ascending) {
	std::sort(_types.begin(), _types.end(), [column, ascending](const auto& t1, const auto& t2) {
		return CompareItems(t1, t2, column, ascending);
		});
	return true;
}

int ManagedTypesView::GetIcon(int row) {
	return (int)_types[row].Kind;
}

PCWSTR ManagedTypesView::GetTypeKind(ManagedTypeKind kind) {
	static PCWSTR kinds[] = {
		L"Attribute",
		L"Class",
		L"Delegate",
		L"Enum",
		L"Interface",
		L"Struct",
	};
	return kinds[(int)kind];
}

bool ManagedTypesView::CompareItems(const ManagedType& t1, const ManagedType& t2, int col, bool asc) {
	switch (col) {
		case 0: return SortHelper::SortStrings(t1.Name, t2.Name, asc);
		case 1: return SortHelper::SortNumbers(t1.Kind, t2.Kind, asc);
		case 2: return SortHelper::SortNumbers(t1.Token, t2.Token, asc);
		case 3: return SortHelper::SortStrings(t1.BaseTypeName, t2.BaseTypeName, asc);
		case 4: case 5:
			return SortHelper::SortNumbers(t1.Attributes, t2.Attributes, asc);
	}
	ATLASSERT(false);
	return false;
}

