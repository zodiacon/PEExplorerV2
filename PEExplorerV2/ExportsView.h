#pragma once

#include "GenericListView.h"
#include "PEParser.h"

class ExportsView : public IGenericListViewCallback {
public:
	ExportsView(PEParser* parser);

	void Init(CListViewCtrl& lv);

	// Inherited via IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

private:
	static bool CompareItems(const ExportedSymbol& e1, const ExportedSymbol& e2, int col, bool asc);

private:
	PEParser* _parser;
	std::vector<ExportedSymbol> _exports;
};

