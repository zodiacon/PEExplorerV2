#include "pch.h"
#include "ImageOptionalHeaderStruct.h"
#include "PEStrings.h"

ImageOptionalHeaderStruct::ImageOptionalHeaderStruct(PEParser* parser) : _parser(parser) {
	BuildMembers();
}

CString ImageOptionalHeaderStruct::GetName() const {
	return L"IMAGE_OPTIONAL_HEADER";
}

int ImageOptionalHeaderStruct::GetMemberCount() const {
	return static_cast<int>(_members.size());
}

const MemberInfo& ImageOptionalHeaderStruct::GetMemberInfo(int index) const {
	return _members[index];
}

void ImageOptionalHeaderStruct::BuildMembers() {
	if (_parser->IsPe64())
		BuildMemberInternal64();
	else
		BuildMemberInternal32();
}

void ImageOptionalHeaderStruct::BuildMemberInternal32() {
	auto& header = _parser->GetOptionalHeader32();

	_members = std::vector<MemberInfo>{
		MemberInfo(L"Magic",						0,	L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.Magic)),
		MemberInfo(L"MajorLinkerVersion",			2,	L"BYTE", sizeof(BYTE), PEStrings::ToDecAndHex(header.MajorLinkerVersion)),
		MemberInfo(L"MinorLinkerVersion",			3,	L"BYTE", sizeof(BYTE), PEStrings::ToDecAndHex(header.MinorLinkerVersion)),
		MemberInfo(L"SizeOfCode",					4,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfCode)),
		MemberInfo(L"SizeOfInitializedData",		8,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfInitializedData)),
		MemberInfo(L"SizeOfUninitializedData",		12,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfUninitializedData)),
		MemberInfo(L"AddressOfEntryPoint",			16,	L"DWORD", sizeof(DWORD), PEStrings::ToHex(header.AddressOfEntryPoint)),
		MemberInfo(L"BaseOfCode",					20,	L"DWORD", sizeof(DWORD), PEStrings::ToHex(header.BaseOfCode)),
		MemberInfo(L"ImageBase",					24,	L"DWORD", sizeof(DWORD), PEStrings::ToHex(header.ImageBase)),
		MemberInfo(L"SectionAlignment",				28,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SectionAlignment)),
		MemberInfo(L"FileAlignment",				32,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.FileAlignment)),
		MemberInfo(L"MajorOperatingSystemVersion",	36, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MajorOperatingSystemVersion)),
		MemberInfo(L"MinorOperatingSystemVersion",	38, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MinorOperatingSystemVersion)),
		MemberInfo(L"MajorImageVersion",			40, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MajorImageVersion)),
		MemberInfo(L"MinorImageVersion",			42, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MinorImageVersion)),
		MemberInfo(L"MajorSubsystemVersion",		44, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MajorSubsystemVersion)),
		MemberInfo(L"MinorSubsystemVersion",		46, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MinorSubsystemVersion)),
		MemberInfo(L"Win32VersionValue",			48, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.Win32VersionValue)),
		MemberInfo(L"SizeOfImage",					52, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfImage)),
		MemberInfo(L"SizeOfHeaders",				56, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfHeaders)),
		MemberInfo(L"Checksum",						60, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.CheckSum)),
		MemberInfo(L"Subsystem",					64, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.Subsystem)),
		MemberInfo(L"DllCharacteristics",			66, L"DWORD", sizeof(WORD), PEStrings::ToDecAndHex(header.DllCharacteristics)),
		MemberInfo(L"SizeOfStackReserve",			68, L"DWORD", sizeof(DWORD), PEStrings::ToMemorySize(header.SizeOfStackReserve)),
		MemberInfo(L"SizeOfStackCommit",			72, L"DWORD", sizeof(DWORD), PEStrings::ToMemorySize(header.SizeOfStackCommit)),
		MemberInfo(L"SizeOfHeapReserve",			76, L"DWORD", sizeof(DWORD), PEStrings::ToMemorySize(header.SizeOfHeapReserve)),
		MemberInfo(L"SizeOfHeapCommit",				80, L"DWORD", sizeof(DWORD), PEStrings::ToMemorySize(header.SizeOfHeapCommit)),
		MemberInfo(L"LoaderFlags",					84, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.LoaderFlags)),
		MemberInfo(L"NumberOfRvaAndSizes",			88, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.NumberOfRvaAndSizes)),
	};
}

void ImageOptionalHeaderStruct::BuildMemberInternal64() {
	auto& header = _parser->GetOptionalHeader64();

	_members = std::vector<MemberInfo>{
		MemberInfo(L"Magic",						0,	L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.Magic)),
		MemberInfo(L"MajorLinkerVersion",			2,	L"BYTE", sizeof(BYTE), PEStrings::ToDecAndHex(header.MajorLinkerVersion)),
		MemberInfo(L"MinorLinkerVersion",			3,	L"BYTE", sizeof(BYTE), PEStrings::ToDecAndHex(header.MinorLinkerVersion)),
		MemberInfo(L"SizeOfCode",					4,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfCode)),
		MemberInfo(L"SizeOfInitializedData",		8,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfInitializedData)),
		MemberInfo(L"SizeOfUninitializedData",		12,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfUninitializedData)),
		MemberInfo(L"AddressOfEntryPoint",			16,	L"DWORD", sizeof(DWORD), PEStrings::ToHex(header.AddressOfEntryPoint)),
		MemberInfo(L"BaseOfCode",					20,	L"DWORD", sizeof(DWORD), PEStrings::ToHex(header.BaseOfCode)),
		MemberInfo(L"ImageBase",					24,	L"ULONGLONG", sizeof(ULONGLONG), PEStrings::ToHex(header.ImageBase)),
		MemberInfo(L"SectionAlignment",				32,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SectionAlignment)),
		MemberInfo(L"FileAlignment",				36,	L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.FileAlignment)),
		MemberInfo(L"MajorOperatingSystemVersion",	40, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MajorOperatingSystemVersion)),
		MemberInfo(L"MinorOperatingSystemVersion",	42, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MinorOperatingSystemVersion)),
		MemberInfo(L"MajorImageVersion",			44, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MajorImageVersion)),
		MemberInfo(L"MinorImageVersion",			46, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MinorImageVersion)),
		MemberInfo(L"MajorSubsystemVersion",		48, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MajorSubsystemVersion)),
		MemberInfo(L"MinorSubsystemVersion",		50, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.MinorSubsystemVersion)),
		MemberInfo(L"Win32VersionValue",			52, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.Win32VersionValue)),
		MemberInfo(L"SizeOfImage",					56, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfImage)),
		MemberInfo(L"SizeOfHeaders",				60, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.SizeOfHeaders)),
		MemberInfo(L"Checksum",						64, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.CheckSum)),
		MemberInfo(L"Subsystem",					68, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.Subsystem)),
		MemberInfo(L"DllCharacteristics",			70, L"DWORD", sizeof(WORD), PEStrings::ToDecAndHex(header.DllCharacteristics)),
		MemberInfo(L"SizeOfStackReserve",			72, L"ULONGLONG", sizeof(ULONGLONG), PEStrings::ToMemorySize(header.SizeOfStackReserve)),
		MemberInfo(L"SizeOfStackCommit",			80, L"ULONGLONG", sizeof(ULONGLONG), PEStrings::ToMemorySize(header.SizeOfStackCommit)),
		MemberInfo(L"SizeOfHeapReserve",			88, L"ULONGLONG", sizeof(ULONGLONG), PEStrings::ToMemorySize(header.SizeOfHeapReserve)),
		MemberInfo(L"SizeOfHeapCommit",				96, L"ULONGLONG", sizeof(ULONGLONG), PEStrings::ToMemorySize(header.SizeOfHeapCommit)),
		MemberInfo(L"LoaderFlags",					104, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.LoaderFlags)),
		MemberInfo(L"NumberOfRvaAndSizes",			108, L"DWORD", sizeof(DWORD), PEStrings::ToDecAndHex(header.NumberOfRvaAndSizes)),
	};
}
