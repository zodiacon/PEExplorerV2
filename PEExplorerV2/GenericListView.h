#pragma once

#include "VirtualListView.h"

struct IGenericListViewCallback {
	virtual int GetItemCount() = 0;
	virtual void Init(CListViewCtrl& lv) = 0;
	virtual CString GetItemText(int row, int col) = 0;
	virtual int GetIcon(int row) {
		return -1;
	}
	virtual bool Sort(int column, bool ascending) = 0;
	virtual bool CanSort(int column) const {
		return true;
	}
	virtual void OnContextMenu(const POINT& pt, int selected) {}
	virtual void ExecuteCommand(WORD id) {}
	virtual bool CanExecuteCommand(WORD id) {
		return true;
	}
};

class CGenericListView :
	public CVirtualListView<CGenericListView>,
	public CWindowImpl<CGenericListView, CListViewCtrl> {
public:
	DECLARE_WND_SUPERCLASS(nullptr, CListViewCtrl::GetWndClassName())

	CGenericListView(IGenericListViewCallback* cb, bool autoDelete = false);

	BEGIN_MSG_MAP(CGenericListView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_GETDISPINFO, OnGetDispInfo)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
		CHAIN_MSG_MAP_ALT(CVirtualListView<CGenericListView>, 1)

	ALT_MSG_MAP(1)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
	END_MSG_MAP()

	void DoSort(const SortInfo* si);
	bool IsSortable(int column) const;
	void Refresh();

private:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	IGenericListViewCallback* m_Callback;
	bool m_AutoDelete : 1;
};

