#pragma once

#include "GenericListView.h"
#include "PEParser.h"
#include "IMainFrame.h"

class SectionsView : public IGenericListViewCallback {
public:
	SectionsView(PEParser* parser, IMainFrame* frame);

	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	void OnContextMenu(const POINT& pt, int index) override;

private:
	CString GetSectionCharacteristics(DWORD ch) const;
	static CString GetSectionName(const IMAGE_SECTION_HEADER* sec);
	bool CompareItems(const PIMAGE_SECTION_HEADER s1, const PIMAGE_SECTION_HEADER s2, int col, bool asc) const;

private:
	IMainFrame* _frame;
	PEParser* _parser;
	std::unique_ptr<PIMAGE_SECTION_HEADER[]> _sections;
};

