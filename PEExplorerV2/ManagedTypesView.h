#pragma once

#include "GenericListView.h"
#include "PEParser.h"
#include "IMainFrame.h"

class ManagedTypesView : public IGenericListViewCallback {
public:
	ManagedTypesView(PEParser* parser, IMainFrame* frame);
	void Init(CListViewCtrl& lv);

	static PCWSTR GetTypeKind(ManagedTypeKind kind);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;
	void OnContextMenu(const POINT& pt, int selected) override;

private:
	static bool CompareItems(const ManagedType& t1, const ManagedType& t2, int col, bool asc);

private:
	PEParser* _parser;
	std::vector<ManagedType> _types;
	IMainFrame* _frame;
};

