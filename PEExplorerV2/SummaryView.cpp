#include "pch.h"
#include "SummaryView.h"
#include "PEParser.h"
#include "PEStrings.h"
#include <algorithm>
#include "SortHelper.h"

SummaryView::SummaryView(PEParser* parser) : _parser(parser) {
}

void SummaryView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 140);
	lv.InsertColumn(1, L"Value", LVCFMT_LEFT, 170);
	lv.InsertColumn(2, L"Details", LVCFMT_LEFT, 500);
}

int SummaryView::GetItemCount() {
	_items.reserve(24);
	auto& fh = _parser->GetFileHeader();

	auto objectFile = _parser->IsObjectFile();
	if (objectFile) {
		_items.push_back(Item(L"Time/Date Stamp", PEStrings::ToHex(fh.TimeDateStamp)));	// , CTime(fh.TimeDateStamp).Format(L"%c")));
		_items.push_back(Item(L"Sections", std::to_wstring(_parser->GetSectionCount()).c_str()));
		_items.push_back(Item(L"Machine", PEStrings::ToDecAndHex(fh.Machine), PEStrings::MachineTypeToString((MachineType)fh.Machine)));
		_items.push_back(Item(L"Image Characteristics", PEStrings::ToDecAndHex(fh.Characteristics), PEStrings::CharacteristicsToString((ImageCharacteristics)fh.Characteristics)));
	}
	else {
		auto& oh32 = _parser->GetOptionalHeader32();
		auto& oh64 = _parser->GetOptionalHeader64();
		auto pe64 = _parser->IsPe64();
		auto magic = pe64 ? oh64.Magic : oh32.Magic;
		auto dllChar = pe64 ? oh64.DllCharacteristics : oh32.DllCharacteristics;

		_items.push_back(Item(L"Time/Date Stamp", PEStrings::ToHex(fh.TimeDateStamp)));	// , CTime(fh.TimeDateStamp).Format(L"%c")));
		_items.push_back(Item(L"Subsystem", std::to_wstring((short)_parser->GetSubsystemType()).c_str(), PEStrings::SubsystemTypeToString(_parser->GetSubsystemType())));
		_items.push_back(Item(L"Sections", std::to_wstring(_parser->GetSectionCount()).c_str()));
		_items.push_back(Item(L"Magic", PEStrings::ToDecAndHex(magic), PEStrings::MagicToString((OptionalHeaderMagic)magic)));
		_items.push_back(Item(L"Machine", PEStrings::ToDecAndHex(fh.Machine), PEStrings::MachineTypeToString((MachineType)fh.Machine)));
		_items.push_back(Item(L"Image Characteristics", PEStrings::ToDecAndHex(fh.Characteristics), PEStrings::CharacteristicsToString((ImageCharacteristics)fh.Characteristics)));
		_items.push_back(Item(L"DLL Characteristics", PEStrings::ToDecAndHex(dllChar), PEStrings::DllCharacteristicsToString((DllCharacteristics)dllChar)));
		_items.push_back(Item(L"Is Managed", _parser->IsManaged() ? L"Yes" : L"No"));
		_items.push_back(Item(L"Image Base", PEStrings::ToHex(pe64 ? oh64.ImageBase : oh32.ImageBase)));
		_items.push_back(Item(L"Heap Commit", PEStrings::ToMemorySize(pe64 ? oh64.SizeOfHeapCommit : oh32.SizeOfHeapCommit)));
		_items.push_back(Item(L"Heap Reserve", PEStrings::ToMemorySize(pe64 ? oh64.SizeOfHeapReserve : oh32.SizeOfHeapReserve)));
		_items.push_back(Item(L"Stack Commit", PEStrings::ToMemorySize(pe64 ? oh64.SizeOfStackCommit : oh32.SizeOfStackCommit)));
		_items.push_back(Item(L"Stack Reserve", PEStrings::ToMemorySize(pe64 ? oh64.SizeOfStackReserve : oh32.SizeOfStackReserve)));
		_items.push_back(Item(L"Code Size", PEStrings::ToMemorySize(pe64 ? oh64.SizeOfCode : oh32.SizeOfCode)));
		_items.push_back(Item(L"Entry Point", PEStrings::ToHex(pe64 ? oh64.AddressOfEntryPoint : oh32.AddressOfEntryPoint)));
		_items.push_back(Item(L"Image Size", PEStrings::ToMemorySize(pe64 ? oh64.SizeOfImage : oh32.SizeOfImage)));
		_items.push_back(Item(L"OS Version", (std::to_wstring(pe64 ? oh64.MajorOperatingSystemVersion : oh32.MajorOperatingSystemVersion) + L"." +
			std::to_wstring(pe64 ? oh64.MinorOperatingSystemVersion : oh32.MinorOperatingSystemVersion)).c_str()));
		_items.push_back(Item(L"Image Version", (std::to_wstring(pe64 ? oh64.MajorImageVersion : oh32.MajorImageVersion) + L"." +
			std::to_wstring(pe64 ? oh64.MinorImageVersion : oh32.MinorImageVersion)).c_str()));
		_items.push_back(Item(L"Linker Version", (std::to_wstring(pe64 ? oh64.MajorLinkerVersion : oh32.MajorLinkerVersion) + L"." +
			std::to_wstring(pe64 ? oh64.MinorLinkerVersion : oh32.MinorLinkerVersion)).c_str()));
		_items.push_back(Item(L"Loader Flags", PEStrings::ToDecAndHex(pe64 ? oh64.LoaderFlags : oh32.LoaderFlags)));
	}
	return static_cast<int>(_items.size());
}

CString SummaryView::GetItemText(int row, int col) {
	switch (col) {
		case 0: return _items[row].Name;
		case 1: return _items[row].Value;
		case 2: return _items[row].Details;
	}
	ATLASSERT(false);
	return L"";
}

bool SummaryView::Sort(int column, bool ascending) {
	std::sort(_items.begin(), _items.end(), [ascending](auto& i1, auto& i2) {
		return SortHelper::SortStrings(i1.Name, i2.Name, ascending);
		});
	return true;
}

bool SummaryView::CanSort(int column) const {
	return column == 0;
}

SummaryView::Item::Item(PCWSTR name, PCWSTR value, PCWSTR details) 
	: Name(name), Value(value), Details(details) {
}
