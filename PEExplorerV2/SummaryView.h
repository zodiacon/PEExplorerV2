#pragma once

#include "GenericListView.h"

class PEParser;

class SummaryView : public IGenericListViewCallback {
	struct Item {
		CString Name, Value, Details;

		Item(PCWSTR name, PCWSTR value, PCWSTR details = nullptr);
	};

public:
	SummaryView(PEParser* parser);
	void Init(CListViewCtrl& lv);
	
	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	bool CanSort(int column) const override;

private:
	PEParser* _parser;
	std::vector<Item> _items;
};

