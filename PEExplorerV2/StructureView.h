#pragma once

#include "GenericListView.h"
#include "PEParser.h"

enum class StructureType {
	DosHeader,
	FileHeader,
	OptionalHeader
};

class StructureView : public IGenericListViewCallback {
public:
	StructureView(PEParser* parser, StructureType type);
	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;
	bool CanSort(int) const override {
		return false;
	}

private:
	PEParser* _parser;
	StructureType _type;
};

