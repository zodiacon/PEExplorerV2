#pragma once

#include "GenericListView.h"
#include "PEParser.h"

class ImportsFunctionsView : public IGenericListViewCallback {
public:
	void SetSymbols(const std::vector<ImportedSymbol>& symbols);
	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

private:
	static bool CompareItems(const ImportedSymbol& s1, const ImportedSymbol& s2, int col, bool asc);

private:
	std::vector<ImportedSymbol> _symbols;
};

