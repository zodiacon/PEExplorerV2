#pragma once

#include "GenericListView.h"
#include "PEParser.h"

class ImportsView : public IGenericListViewCallback {
public:
	ImportsView(PEParser* parser);
	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;

private:
	struct ImportLibrary {
		CString Name;
		int Count;
	};

	PEParser* _parser;
	std::vector<ImportLibrary> _libraries;
};

