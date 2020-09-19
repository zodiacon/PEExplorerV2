#include "pch.h"
#include "AssemblyView.h"
#include "PEStrings.h"

CAssemblyView::CAssemblyView(cs_insn* insts, int count) : 
	m_Inst(insts), m_Count(count) {
}

LRESULT CAssemblyView::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
	m_Edit.Create(*this, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | WS_VSCROLL | WS_CLIPSIBLINGS);
	CFont font;
	font.CreatePointFont(110, L"Consolas");
	m_Edit.SetFont(font.Detach());

	CString text;
	for (int j = 0; j < m_Count; j++) {
		const auto& inst = m_Inst[j];
		text += PEStrings::FormatInstruction(inst) + L"\r\n";
		if (_stricmp(inst.mnemonic, "ret") == 0 || inst.bytes[0] == 0xcc)
			break;
	}
	m_Edit.SetWindowText(text);

	return 0;
}

LRESULT CAssemblyView::OnSize(UINT, WPARAM, LPARAM lParam, BOOL&) {
	if (m_Edit) {
		m_Edit.MoveWindow(0, 0, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	return 0;
}

LRESULT CAssemblyView::OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&) {
	return 1;
}

LRESULT CAssemblyView::OnBackColor(UINT, WPARAM, LPARAM, BOOL&) {
	return reinterpret_cast<LRESULT>(::GetSysColorBrush(COLOR_WINDOW));
}
