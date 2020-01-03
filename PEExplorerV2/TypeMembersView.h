#pragma once

#include "GenericListView.h"
#include "PEParser.h"

class TypeMembersView : public IGenericListViewCallback {
public:
	TypeMembersView(CLRMetadataParser& parser, const ManagedType& type);

	static PCWSTR MemberTypeToString(ManagedMemberType type);

	// IGenericListViewCallback
	int GetItemCount() override;
	void Init(CListViewCtrl& lv) override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;
	bool CanSort(int col) const override;

	CString GetMoreInfo(const ManagedMember& member) const;

private:
	bool CompareItems(const ManagedMember& m1, const ManagedMember& m2, int col, bool asc) const;

private:
	ManagedType _type;
	std::vector<ManagedMember> _members;
	CLRMetadataParser& _parser;
};

