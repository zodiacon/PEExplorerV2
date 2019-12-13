#pragma once

#include "GenericListView.h"
#include "PEParser.h"
#include "IMainFrame.h"

class ExportsView : public IGenericListViewCallback {
public:
	ExportsView(PEParser* parser, IMainFrame* frame);

	void Init(CListViewCtrl& lv);

	// Inherited via IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;
	void OnContextMenu(const POINT& pt, int selected) override;

private:
	static bool CompareItems(const ExportedSymbol& e1, const ExportedSymbol& e2, int col, bool asc);

private:
	PEParser* _parser;
	IMainFrame* _frame;
	std::vector<ExportedSymbol> _exports;
};

