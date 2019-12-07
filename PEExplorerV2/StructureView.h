#pragma once

#include "GenericListView.h"
#include "PEParser.h"

struct MemberInfo {
	CString Name;
	unsigned Offset;
	CString Type;
	int Size;
	CString Value;
	CString Comment;

	MemberInfo(PCWSTR name, unsigned offset, PCWSTR type, int size, PCWSTR value, PCWSTR comment = nullptr);
};

struct IStructureProvider {
	virtual CString GetName() const = 0;
	virtual int GetMemberCount() const = 0;
	virtual const MemberInfo& GetMemberInfo(int index) const = 0;
};

class StructureView : public IGenericListViewCallback {
public:
	StructureView(PEParser* parser, IStructureProvider* provider);
	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	bool CanSort(int) const override {
		return false;
	}

private:
	PEParser* _parser;
	std::unique_ptr<IStructureProvider> _provider;
};

