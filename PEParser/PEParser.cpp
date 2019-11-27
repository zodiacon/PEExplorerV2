// PEParser.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "PEParser.h"
#include <ImageHlp.h>

#pragma comment(lib, "imagehlp")

PEParser::PEParser(const char* path) : PEParser(CString(path)) {
}

PEParser::PEParser(const wchar_t* path) {
	_address = (PBYTE)::LoadLibraryEx(path, nullptr, LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
	if (_address == nullptr)
		return;

	_address = (PBYTE)(((ULONG_PTR)_address) & ~0xf);

	CheckValidity();
}

PEParser::~PEParser() {
	if (_address)
		::FreeLibrary((HMODULE)_address);
	if (_hMemFile)
		::CloseHandle(_hMemFile);
}

bool PEParser::IsValid() const {
	return _valid;
}

bool PEParser::IsPe64() const {
	return _opt32->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;
}

bool PEParser::IsExecutable() const {
	if (!IsValid())
		return false;

	return (_fileHeader->Characteristics & IMAGE_FILE_DLL) == 0;
}

bool PEParser::IsManaged() const {
	return GetDataDirectory(IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR)->Size != 0;
}

int PEParser::GetSectionCount() const {
	if (!IsValid())
		return -1;

	return _fileHeader->NumberOfSections;
}

const IMAGE_SECTION_HEADER* PEParser::GetSectionHeader(ULONG section) const {
	if (!IsValid() || section >= _fileHeader->NumberOfSections)
		return nullptr;

	return _sections + section;
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
			ImportedSymbol symbol;
			symbol.Hint = *(unsigned short*)p;
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
	return *_fileHeader;
}

void* PEParser::GetAddress(unsigned rva) const {
	if (!IsValid())
		return nullptr;

	//auto sections = _sections;
	//int count = GetSectionCount();
	//for (int i = 0; i < count; ++i) {
	//	if (rva >= sections[i].VirtualAddress && rva < sections[i].VirtualAddress + sections[i].SizeOfRawData)
	//		return _address + sections[i].PointerToRawData + rva - sections[i].VirtualAddress;
	//}

	return _address + rva;

}

SubsystemType PEParser::GetSubsystemType() const {
	return static_cast<SubsystemType>(IsPe64() ? GetOptionalHeader64().Subsystem : GetOptionalHeader32().Subsystem);
}

void PEParser::CheckValidity() {
	_dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(_address);
	if (_dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return;

	auto ntHeader = (IMAGE_NT_HEADERS*)((PBYTE)_dosHeader + _dosHeader->e_lfanew);
	_fileHeader = &ntHeader->FileHeader;
	_opt32 = (IMAGE_OPTIONAL_HEADER32*)&ntHeader->OptionalHeader;
	_opt64 = (IMAGE_OPTIONAL_HEADER64*)&ntHeader->OptionalHeader;
	_sections = IsPe64() ? (PIMAGE_SECTION_HEADER)(_opt64 + 1) : (PIMAGE_SECTION_HEADER)(_opt32 + 1);
	_valid = true;
}

unsigned PEParser::RvaToFileOffset(unsigned rva) const {
	auto sections = _sections;
	for (int i = 0; i < GetSectionCount(); ++i) {
		if (rva >= sections[i].VirtualAddress && rva < sections[i].VirtualAddress + _sections[i].Misc.VirtualSize)
			return sections[i].PointerToRawData + rva - sections[i].VirtualAddress;
	}

	return rva;
}

//void PEParser::ParseResourceDirectoryEntry(IMAGE_RESOURCE_DIRECTORY_ENTRY* entry, void* root, const CString& parent, IResourceCallback* cb) const {
//	CString name;
//	if (entry->NameIsString) {
//		name = GetResourceName((PBYTE)root + entry->NameOffset);
//	}
//	else {
//		name = (L"#" + std::to_wstring(entry->Id)).c_str();
//	}
//	if (entry->DataIsDirectory) {
//		cb->OnResourceDirectory(name, parent);
//		ParseResourceDirectory((IMAGE_RESOURCE_DIRECTORY*)((PBYTE)root + entry->OffsetToDirectory), root, parent + L"/" + name, cb);
//	}
//	else {
//		auto data = reinterpret_cast<IMAGE_RESOURCE_DATA_ENTRY*>((PBYTE)root + entry->OffsetToData);
//		cb->OnResource(name, (PBYTE)entry + data->OffsetToData, data->Size);
//	}
//}
//
//void PEParser::ParseResourceDirectory(IMAGE_RESOURCE_DIRECTORY* dir, void* root, const CString& parent, IResourceCallback* cb) const {
//	auto namedCount = dir->NumberOfNamedEntries;
//	auto idCount = dir->NumberOfIdEntries;
//
//	auto entry = reinterpret_cast<IMAGE_RESOURCE_DIRECTORY_ENTRY*>(dir + 1);
//	for (int i = 0; i < namedCount + idCount; i++) {
//		ParseResourceDirectoryEntry(entry, root, parent, cb);
//		entry++;
//	}
//}

CString PEParser::GetResourceName(void* data) const {
	auto sdata = (IMAGE_RESOURCE_DIR_STRING_U*)data;
	return CString(sdata->NameString, sdata->Length);
}

std::vector<ResourceType> PEParser::EnumResources() const {
	using Types = std::vector<ResourceType>;
	Types types;
	if (!IsValid())
		return types;

	auto dir = GetDataDirectory(IMAGE_DIRECTORY_ENTRY_RESOURCE);
	if (dir->Size == 0)
		return types;

	types.reserve(8);
	struct Context {
		Types* Types;
		HMODULE Module;
	} context;

	context.Module = (HMODULE)(_address + 2);
	context.Types = &types;

	::EnumResourceTypes((HMODULE)(_address + 2), [](auto h, auto type, auto p) {
		auto context = reinterpret_cast<Context*>(p);
		ResourceType rt;
		if ((ULONG_PTR)type < 0x10000)
			rt.Name = (L"#" + std::to_wstring((WORD)reinterpret_cast<ULONG_PTR>(type))).c_str();
		else
			rt.Name = type;

		::EnumResourceNames(context->Module, type, [](auto, auto type, auto name, auto p) {
			auto rt = reinterpret_cast<ResourceType*>(p);

			CString resName;
			if((ULONG_PTR)name < 0x10000)
				resName = (L"#" + std::to_wstring((WORD)reinterpret_cast<ULONG_PTR>(name))).c_str();
			else
				resName = name;
			rt->Items.push_back(std::move(resName));
			return TRUE;
			}, reinterpret_cast<LONG_PTR>(&rt));

		context->Types->push_back(rt);

		return TRUE;
		}, reinterpret_cast<LONG_PTR>(&context));

	return types;
}
