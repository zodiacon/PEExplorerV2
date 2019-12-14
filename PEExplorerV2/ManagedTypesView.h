#pragma once

#include "GenericListView.h"
#include "PEParser.h"

class ManagedTypesView : public IGenericListViewCallback {
public:
	ManagedTypesView(PEParser* parser);
	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

private:
	static PCWSTR GetTypeKind(ManagedTypeKind kind);
	static bool CompareItems(const ManagedType& t1, const ManagedType& t2, int col, bool asc);

private:
	PEParser* _parser;
	std::vector<ManagedType> _types;
};

