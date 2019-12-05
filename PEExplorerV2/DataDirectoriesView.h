#pragma once

#include "GenericListView.h"
#include "PEParser.h"
#include "IMainFrame.h"

class DataDirectoriesView : public IGenericListViewCallback {
public:
	DataDirectoriesView(PEParser* parser, IMainFrame* frame);
	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	bool CanSort(int col) const override {
		return false;
	}
	void OnContextMenu(const POINT& pt, int index) override;

private:
	struct DataDirectory {
		CString Name;
		int Index;
		DWORD Address;
		DWORD Size;
	};
	PEParser* _parser;
	DataDirectory _dirs[15];
	IMainFrame* _frame;
};

