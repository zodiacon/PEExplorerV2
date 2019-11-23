#include "pch.h"
#include "PEParser.h"

#pragma comment(lib, "imagehlp")

PEParser::PEParser(const char* path) {
	_image = ::ImageLoad(path, nullptr);
}

PEParser::~PEParser() {
	if (_image)
		::ImageUnload(_image);
}

bool PEParser::IsValid() const {
	return _image != nullptr;
}

bool PEParser::IsPe64() const {
	return _image->FileHeader->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;
}

bool PEParser::IsExecutable() const {
	if (!IsValid())
		return false;

	return (_image->FileHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0;
}

int PEParser::GetSectionCount() const {
	if (!IsValid())
		return -1;

	return _image->NumberOfSections;
}

const IMAGE_SECTION_HEADER* PEParser::GetSectionHeader(ULONG section) const {
	if (!IsValid() || section >= _image->NumberOfSections)
		return nullptr;

	return &_image->Sections[section];
}


const IMAGE_DATA_DIRECTORY* PEParser::GetDataDirectory(int index) const {
	if (!IsValid() || index < 0 || index > 15)
		return nullptr;

	if (IsPe64())
		return &GetOptionalHeader64().DataDirectory[index];
	return &GetOptionalHeader32().DataDirectory[index];
}

std::vector<ExportedSymbol> PEParser::GetExports() const {
	std::vector<ExportedSymbol> exports;
	auto dir = GetDataDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT);
	if (dir == nullptr || dir->Size == 0)
		return exports;

	auto data = static_cast<IMAGE_EXPORT_DIRECTORY*>(GetAddress(dir->VirtualAddress));
	auto count = data->NumberOfNames;
	exports.reserve(count);

	auto names = (PBYTE)(data->AddressOfNames != 0 ? GetAddress(data->AddressOfNames) : nullptr);
	auto ordinals = (PBYTE)(data->AddressOfNameOrdinals != 0 ? GetAddress(data->AddressOfNameOrdinals) : nullptr);
	auto functions = GetAddress(data->AddressOfFunctions);
	char undecorated[1 << 10];
	auto ordinalBase = data->Base;

	for (DWORD i = 0; i < data->NumberOfNames; i++) {
		ExportedSymbol symbol;
		if (names) {
			auto offset = *(ULONG*)(names + i * 4);
			symbol.Name = (PCSTR)GetAddress(offset);
			if (::UnDecorateSymbolName(symbol.Name.c_str(), undecorated, sizeof(undecorated), 0))
				symbol.UndecoratedName = undecorated;
		}
		auto address = *(DWORD*)((PBYTE)functions + i * 4);
		symbol.Address = address;
		auto offset = RvaToFileOffset(address);
		if (offset > dir->VirtualAddress&& offset < dir->VirtualAddress + dir->Size) {
			symbol.ForwardName = (PCSTR)GetAddress(address);
		}

		if (ordinals) {
			symbol.Ordinal = *(USHORT*)(ordinals + i * 2);
		}
		else {
			symbol.Ordinal = 0xffff;
		}
		exports.push_back(std::move(symbol));
	}

	return exports;
}

std::vector<ImportedLibrary> PEParser::GetImports() const {
	std::vector<ImportedLibrary> libs;

	if (!IsValid())
		return libs;

	auto dir = GetDataDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (dir->Size == 0)
		return libs;

	struct IMAGE_IMPORT_DIRECTORY {
		int ImportLookupTable;
		int TimeDateStamp;
		int ForwarderChain;
		int NameRva;
		int ImportAddressTable;
	};

	auto imports = static_cast<IMAGE_IMPORT_DIRECTORY*>(GetAddress(dir->VirtualAddress));
	auto inc = IsPe64() ? 8 : 4;
	char undecorated[1 << 10];

	for (;;) {
		auto offset = imports->ImportLookupTable == 0 ? imports->ImportAddressTable : imports->ImportLookupTable;
		if (offset == 0)
			break;
		auto iat = static_cast<PBYTE>(GetAddress(offset));
		auto hintNameTable = static_cast<PBYTE>(GetAddress(imports->ImportAddressTable));
		auto libName = static_cast<PBYTE>(GetAddress(imports->NameRva));

		ImportedLibrary lib;
		lib.Name = (PCSTR)libName;

		for (;;) {
			int ordinal = -1;
			int nameRva = 0;
			if (inc == 8) {
				auto value = *(ULONGLONG*)iat;
				if (value == 0)
					break;
				auto isOrdinal = (value & (1ULL << 63)) > 0;
				if (isOrdinal)
					ordinal = (unsigned short)(value & 0xffff);
				else
					nameRva = (int)(value & ((1LL << 31) - 1));
			}
			else {
				auto value = *(ULONG*)iat;
				if (value == 0)
					break;
				auto isOrdinal = (value & (1UL << 31)) > 0;
				if (isOrdinal)
					ordinal = (unsigned short)(value & 0xffff);
				else
					nameRva = (int)(value & ((1LL << 31) - 1));
			}
			if (nameRva == 0)
				break;
			auto p = static_cast<PBYTE>(GetAddress(nameRva));
			auto hint = *(unsigned short*)p;
			ImportedSymbol symbol;
			symbol.Name = (PCSTR)(p + 2);
			if (::UnDecorateSymbolName(symbol.Name.c_str(), undecorated, sizeof(undecorated), 0))
				symbol.UndecoratedName = undecorated;

			lib.Symbols.push_back(std::move(symbol));

			iat += inc;
		}
		libs.push_back(std::move(lib));
		imports++;
	}
	return libs;
}

const IMAGE_FILE_HEADER& PEParser::GetFileHeader() const {
	return _image->FileHeader->FileHeader;
}

void* PEParser::GetAddress(unsigned rva) const {
	if (!IsValid())
		return nullptr;

	auto sections = _image->Sections;
	int count = _image->NumberOfSections;
	for (int i = 0; i < count; ++i) {
		if (rva >= sections[i].VirtualAddress && rva < sections[i].VirtualAddress + sections[i].SizeOfRawData)
			return _image->MappedAddress + sections[i].PointerToRawData + rva - sections[i].VirtualAddress;
	}

	return _image->MappedAddress + rva;

}

SubsystemType PEParser::GetSubsystemType() const {
	return static_cast<SubsystemType>(IsPe64() ? GetOptionalHeader64().Subsystem : GetOptionalHeader32().Subsystem);
}

CString PEParser::GetFileName() const {
	return _image ? CString(_image->ModuleName) : L"";
}

unsigned PEParser::RvaToFileOffset(unsigned rva) const {
	auto sections = _image->Sections;
	for (int i = 0; i < GetSectionCount(); ++i) {
		if (rva >= sections[i].VirtualAddress && rva < sections[i].VirtualAddress + _image->Sections[i].Misc.VirtualSize)
			return sections[i].PointerToRawData + rva - sections[i].VirtualAddress;
	}

	return rva;
}
