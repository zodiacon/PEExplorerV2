#pragma once

#include "GenericListView.h"
#include "PEParser.h"

struct ResourceItem {
	CString Type;
	ResourceInfo Resource;
};

class ResourcesView : public IGenericListViewCallback {
public:
	ResourcesView(PEParser* parser);
	void Init(CListViewCtrl& lv);

	// IGenericListViewCallback
	int GetItemCount() override;
	CString GetItemText(int row, int col) override;
	bool Sort(int column, bool ascending) override;

	const ResourceItem& GetResource(int index) const;

private:
	static bool CompareItems(const ResourceItem& res1, const ResourceItem& res2, int col, bool asc);

	PEParser* _parser;
	std::vector<ResourceItem> _items;
};

