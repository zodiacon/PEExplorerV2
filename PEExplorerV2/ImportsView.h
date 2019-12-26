#pragma once

#include "GenericListView.h"
#include "PEParser.h"
#include "IMainFrame.h"

class ImportsView : public IGenericListViewCallback {
public:
	ImportsView(PEParser* parser, IMainFrame* frame);
	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	int GetIcon(int row) override;

	const ImportedLibrary& GetLibrary(int selected) const;
	void OnContextMenu(const POINT& pt, int selected) override;

private:
	static bool CompareItems(ImportedLibrary& lib1, ImportedLibrary& lib2, int col, bool asc);

	PEParser* _parser;
	std::vector<ImportedLibrary> _libraries;
	IMainFrame* _frame;
};

