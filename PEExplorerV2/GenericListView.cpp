#include "pch.h"
#include "GenericListView.h"
#include "ClipboardHelper.h"

CGenericListView::CGenericListView(IGenericListViewCallback* cb, bool autoDelete) {
	ATLASSERT(cb);
	m_Callback = cb;
	m_AutoDelete = autoDelete;
}

void CGenericListView::DoSort(const SortInfo* si) {
	m_Callback->Sort(si->SortColumn, si->SortAscending);
	RedrawItems(GetTopIndex(), GetTopIndex() + GetCountPerPage());
}

bool CGenericListView::IsSortable(int column) const {
	return m_Callback->CanSort(column);
}

void CGenericListView::Refresh() {
	SetItemCount(m_Callback->GetItemCount());
}

LRESULT CGenericListView::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
	DefWindowProc();

	SetExtendedListViewStyle(LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);

	m_Callback->Init(*this);

	Refresh();

	return 0;
}

LRESULT CGenericListView::OnDestroy(UINT, WPARAM, LPARAM, BOOL&) {
	if (m_AutoDelete) {
		delete m_Callback;
		m_Callback = nullptr;
	}
	return DefWindowProc();
}

LRESULT CGenericListView::OnGetDispInfo(int, LPNMHDR hdr, BOOL&) {
	auto di = reinterpret_cast<NMLVDISPINFO*>(hdr);
	auto& item = di->item;
	auto row = item.iItem;
	auto col = item.iSubItem;

	if (di->item.mask & LVIF_TEXT) {
		::StringCchCopy(item.pszText, item.cchTextMax, m_Callback->GetItemText(row, col));
	}
	if (di->item.mask & LVIF_IMAGE) {
		item.iImage = m_Callback->GetIcon(row);
	}
	return 0;
}

LRESULT CGenericListView::OnContextMenu(UINT, WPARAM, LPARAM lParam, BOOL&) {
	CPoint pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	auto selected = GetSelectedCount() == 0 ? -1 : GetNextItem(-1, LVIS_SELECTED);
	if (selected >= 0 && pt.x == -1) {	// keyboard 
		CRect rc;
		GetItemRect(selected, &rc, LVIR_LABEL);
		pt = rc.CenterPoint();
	}
	m_Callback->OnContextMenu(pt, selected);

	return 0;
}

LRESULT CGenericListView::OnForwardMsg(UINT, WPARAM, LPARAM lParam, BOOL& handled) {
	auto msg = reinterpret_cast<MSG*>(lParam);
	LRESULT result = 0;
	handled = ProcessWindowMessage(*this, msg->message, msg->wParam, msg->lParam, result, 1);

	return result;
}

LRESULT CGenericListView::OnEditCopy(WORD, WORD, HWND, BOOL&) {
	CString text;
	int i = -1;
	LVCOLUMN col;
	col.mask = LVCF_FMT;
	int max = 0;
	for (;; max++)
		if (!GetColumn(max, &col))
			break;

	while (true) {
		i = GetNextItem(i, LVNI_SELECTED);
		if (i < 0)
			break;

		for (int c = 0; c < max; c++) {
			CString item;
			if (!GetItemText(i, c, item))
				break;
			text += item + L"\t";
		}
		text += L"\n";
	}
	if (!text.IsEmpty())
		ClipboardHelper::CopyText(*this, text);
	return 0;
}

