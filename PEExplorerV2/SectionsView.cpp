#include "pch.h"
#include "SectionsView.h"
#include "PEStrings.h"
#include <algorithm>
#include "SortHelper.h"
#include "resource.h"

SectionsView::SectionsView(PEParser* parser, IMainFrame* frame) 
	: _parser(parser), _frame(frame) {
	_sections = std::make_unique<PIMAGE_SECTION_HEADER[]>(_parser->GetSectionCount());
	for (int i = 0; i < _parser->GetSectionCount(); i++)
		_sections[i] = const_cast<PIMAGE_SECTION_HEADER>(_parser->GetSectionHeader(i));
}

void SectionsView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 100);
	lv.InsertColumn(1, L"Size", LVCFMT_RIGHT, 120);
	lv.InsertColumn(2, L"Address", LVCFMT_RIGHT, 80);
	lv.InsertColumn(3, L"Ptr to Raw Data", LVCFMT_RIGHT, 100);
	lv.InsertColumn(4, L"Raw Data Size", LVCFMT_RIGHT, 120);
	lv.InsertColumn(5, L"Characteristics", LVCFMT_LEFT, 300);
}

int SectionsView::GetItemCount() {
	return _parser->GetSectionCount();
}

CString SectionsView::GetItemText(int row, int col) {
	auto& sec = _sections[row];

	switch (col) {
		case 0: return GetSectionName(sec);
		case 1: return PEStrings::ToMemorySize(sec->Misc.VirtualSize);
		case 2: return PEStrings::ToHex(sec->VirtualAddress);
		case 3: return PEStrings::ToHex(sec->PointerToRawData);
		case 4: return PEStrings::ToMemorySize(sec->SizeOfRawData);
		case 5: return GetSectionCharacteristics(sec->Characteristics);
	}
	return L"";
}

bool SectionsView::Sort(int column, bool ascending) {
	std::sort(_sections.get(), _sections.get() + _parser->GetSectionCount(), [this, column, ascending](auto s1, auto s2) {
		return CompareItems(s1, s2, column, ascending);
		});
	return true;
}

void SectionsView::OnContextMenu(const POINT& pt, int index) {
	CMenuHandle menu;
	menu.LoadMenuW(IDR_CONTEXT);
	auto cmd = _frame->ShowContextMenu(menu.GetSubMenu(0), pt, TPM_RETURNCMD);
	if (cmd == ID_SECTION_VIEWDATA) {
		_frame->CreateHexView(TreeNodeType::SectionView, nullptr, index);
	}
}

CString SectionsView::GetSectionCharacteristics(DWORD c) const {
	CString result;
	auto ch = static_cast<SectionFlags>(c);

	struct {
		SectionFlags Flags;
		PCWSTR Text;
	} items[] = {
		{ SectionFlags::NoPad, L"No Pad" },
		{ SectionFlags::Code, L"Code" },
		{ SectionFlags::Read, L"Read" },
		{ SectionFlags::Write, L"Write" },
		{ SectionFlags::Execute, L"Execute" },
		{ SectionFlags::Shared, L"Shared" },
		{ SectionFlags::InitializedData, L"Initialized Data" },
		{ SectionFlags::UninitializedData, L"Uninitialized Data" },
		{ SectionFlags::Remove, L"Remove" },
		{ SectionFlags::Discardable, L"Discardable" },
		{ SectionFlags::Info, L"Info" },
		{ SectionFlags::Comdat, L"Comdat" },
		{ SectionFlags::GPRel, L"GP Relative" },
		{ SectionFlags::ExtendedReloc, L"Extended Reloc" },
		{ SectionFlags::NotPaged, L"Not Paged" },
		{ SectionFlags::NotCached, L"Not Cached" },
	};

	for (auto& item : items)
		if ((ch & item.Flags) != SectionFlags::None)
			result += item.Text + CString(L", ");

	if (!result.IsEmpty())
		result = result.Left(result.GetLength() - 2);

	result = PEStrings::ToHex(c, true) + L" (" + result + L")";
	return result;
}

CString SectionsView::GetSectionName(const IMAGE_SECTION_HEADER* sec) {
	if (sec->Name[7] != '\0')
		return CString((PCSTR)sec->Name, 8);
	return CString((PCSTR)sec->Name);
}

bool SectionsView::CompareItems(const PIMAGE_SECTION_HEADER s1, const PIMAGE_SECTION_HEADER s2, int col, bool asc) const {
	switch (col) {
		case 0: return SortHelper::SortStrings(GetSectionName(s1), GetSectionName(s2), asc);
		case 1: return SortHelper::SortNumbers(s1->Misc.VirtualSize, s2->Misc.VirtualSize, asc);
		case 2: return SortHelper::SortNumbers(s1->VirtualAddress, s2->VirtualAddress, asc);
		case 3: return SortHelper::SortNumbers(s1->PointerToRawData, s2->PointerToRawData, asc);
		case 4: return SortHelper::SortNumbers(s1->SizeOfRawData, s2->SizeOfRawData, asc);
		case 5: return SortHelper::SortNumbers(s1->Characteristics, s2->Characteristics, asc);
	}
	
	ATLASSERT(false);
	return false;
}

