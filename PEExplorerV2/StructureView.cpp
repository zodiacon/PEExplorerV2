#include "pch.h"
#include "StructureView.h"
#include "PEStrings.h"

StructureView::StructureView(PEParser* parser, IStructureProvider* provider) 
	: _parser(parser), _provider(provider) {
	ATLASSERT(provider);
}

void StructureView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 150);
	lv.InsertColumn(1, L"Offset", LVCFMT_RIGHT, 80);
	lv.InsertColumn(2, L"Type", LVCFMT_LEFT, 100);
	lv.InsertColumn(3, L"Value", LVCFMT_RIGHT, 150);
	lv.InsertColumn(4, L"Comment", LVCFMT_LEFT, 200);
}

int StructureView::GetItemCount() {
	return _provider->GetMemberCount();
}

CString StructureView::GetItemText(int row, int col) {
	auto& info = _provider->GetMemberInfo(row);

	switch (col) {
		case 0:	return info.Name;
		case 1: return PEStrings::ToDecAndHex(info.Offset);
		case 2: return info.Type;
		case 3: return info.Value;
		case 4: return info.Comment;
	}
	ATLASSERT(false);
	return L"";
}

bool StructureView::Sort(int column, bool ascending) {
	return false;
}

MemberInfo::MemberInfo(PCWSTR name, unsigned offset, PCWSTR type, int size, PCWSTR value, PCWSTR comment) 
	: Name(name), Offset(offset), Type(type), Size(size), Value(value), Comment(comment) {
}
