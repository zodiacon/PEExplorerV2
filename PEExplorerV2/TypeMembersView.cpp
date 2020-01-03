#include "pch.h"
#include "TypeMembersView.h"
#include "CLRMetadataParser.h"
#include "PEStrings.h"
#include "resource.h"
#include <algorithm>
#include "SortHelper.h"

TypeMembersView::TypeMembersView(CLRMetadataParser& parser, const ManagedType& type) : _parser(parser), _type(type) {
}

PCWSTR TypeMembersView::MemberTypeToString(ManagedMemberType type) {
	switch (type) {
		case ManagedMemberType::Method: return L"Method";
		case ManagedMemberType::Field: return L"Field";
		case ManagedMemberType::Property: return L"Property";
		case ManagedMemberType::Event: return L"Event";
		case ManagedMemberType::Constructor: return L"Constructor";
		case ManagedMemberType::StaticConstructor: return L"Static Constructor";
	}

	ATLASSERT(false);
	return L"Unknown";
}

int TypeMembersView::GetItemCount() {
	_members = _parser.EnumMembers(_type.Token, true);

	return static_cast<int>(_members.size());
}

void TypeMembersView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 200);
	lv.InsertColumn(1, L"Kind", LVCFMT_LEFT, 70);
	lv.InsertColumn(2, L"Token", LVCFMT_RIGHT, 80);
	lv.InsertColumn(3, L"Declaring Type", LVCFMT_LEFT, 200);
	lv.InsertColumn(4, L"Attributes", LVCFMT_RIGHT, 90);
	lv.InsertColumn(5, L"Attributes Details", LVCFMT_LEFT, 200);
	lv.InsertColumn(6, L"More Information", LVCFMT_LEFT, 250);

	CImageList images;
	images.Create(16, 16, ILC_COLOR32, 6, 0);
	UINT ids[] = { IDI_METHOD, IDI_PROPERTY, IDI_EVENT, IDI_FIELD, IDI_CTOR, IDI_CLASS, IDI_CCTOR };
	for (auto id : ids)
		images.AddIcon(AtlLoadIconImage(id, 0, 16, 16));
	lv.SetImageList(images, LVSIL_SMALL);
}

CString TypeMembersView::GetItemText(int row, int col) {
	auto& item = _members[row];

	switch (col) {
		case 0: return item.Name;
		case 1: return MemberTypeToString(item.Type);
		case 2: return PEStrings::ToHex((ULONG)item.Token);
		case 3: return _parser.GetTypeName(item.ClassToken);
		case 4: return PEStrings::ToHex(item.Attributes);
		case 5: return PEStrings::MemberAttributesToString(item);
		case 6: return GetMoreInfo(item);
	}

	ATLASSERT(false);
	return L"";
}

bool TypeMembersView::Sort(int column, bool ascending) {
	std::sort(_members.begin(), _members.end(), [=](auto& m1, auto& m2) {
		return CompareItems(m1, m2, column, ascending);
		});

	return true;
}

int TypeMembersView::GetIcon(int row) {
	return (int)_members[row].Type;
}

bool TypeMembersView::CanSort(int col) const {
	return col != 6;
}

CString TypeMembersView::GetMoreInfo(const ManagedMember& member) const {
	CString details, name;

	switch (member.Type) {
		case ManagedMemberType::Method:
			break;

		case ManagedMemberType::Property:
			name = _parser.GetMethodName(member.Property.Getter);
			if(!name.IsEmpty())
				details += L"Getter: " + name + L", ";
			name = _parser.GetMethodName(member.Property.Setter);
			if (!name.IsEmpty())
				details += L"Setter: " + name + L", ";

			break;

		case ManagedMemberType::Event:
			if (member.Event.AddMethod)
				details += L"Add: " + _parser.GetMethodName(member.Event.AddMethod) + L", ";
			if (member.Event.RemoveMethod)
				details += L"Remove: " + _parser.GetMethodName(member.Event.RemoveMethod) + L", ";
			if (member.Event.FireMethod)
				details += L"Fire: " + _parser.GetMethodName(member.Event.FireMethod) + L", ";
			break;
	}

	if (!details.IsEmpty())
		details = details.Left(details.GetLength() - 2);
	return details;
}

bool TypeMembersView::CompareItems(const ManagedMember& m1, const ManagedMember& m2, int col, bool asc) const {
	switch (col) {
		case 0: return SortHelper::SortStrings(m1.Name, m2.Name, asc);
		case 1: return SortHelper::SortStrings(MemberTypeToString(m1.Type), MemberTypeToString(m2.Type), asc);
		case 2: return SortHelper::SortNumbers(m1.Token, m2.Token, asc);
		case 3: return SortHelper::SortStrings(_parser.GetTypeName(m1.ClassToken), _parser.GetTypeName(m2.ClassToken), asc);
		case 4: case 5: return SortHelper::SortNumbers(m1.Attributes, m2.Attributes, asc);
	}
	ATLASSERT(false);
	return false;
}
