#pragma once

#include "GenericListView.h"
#include "PEParser.h"

class ResourcesView : public IGenericListViewCallback {
public:
	ResourcesView(PEParser* parser, CTreeItem resTreeItem);
	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;

private:
	struct ResourceItem {
		CString Type;
		ResourceInfo Resource;
	};

	PEParser* _parser;
	std::vector<ResourceItem> _items;
};

