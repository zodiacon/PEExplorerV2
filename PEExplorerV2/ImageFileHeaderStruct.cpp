#include "pch.h"
#include "ImageFileHeaderStruct.h"
#include "PEStrings.h"

ImageFileHeaderStruct::ImageFileHeaderStruct(PEParser* parser) : _parser(parser) {
	BuildMembers();
}

CString ImageFileHeaderStruct::GetName() const {
	return L"IMAGE_FILE_HEADER";
}

int ImageFileHeaderStruct::GetMemberCount() const {
	return static_cast<int>(_members.size());
}

const MemberInfo& ImageFileHeaderStruct::GetMemberInfo(int index) const {
	return _members[index];
}

void ImageFileHeaderStruct::BuildMembers() {
	auto& header = _parser->GetFileHeader();

	_members = std::vector<MemberInfo>{
		MemberInfo(L"Machine",				0,	L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.Machine)),
		MemberInfo(L"NumberOfSections",		2,	L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.NumberOfSections)),
		MemberInfo(L"TImeDateStamp",		4,	L"DWORD", sizeof(DWORD), PEStrings::ToHex(header.TimeDateStamp, true)),
		MemberInfo(L"PointerToSymbolTable",	8,	L"DWORD", sizeof(WORD), PEStrings::ToDecAndHex(header.PointerToSymbolTable)),
		MemberInfo(L"NumberOfSymbols",		12,	L"DWORD", sizeof(WORD), PEStrings::ToDecAndHex(header.NumberOfSymbols)),
		MemberInfo(L"SizeOfOptionalHeader",	16,	L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.SizeOfOptionalHeader)),
		MemberInfo(L"Characteristics",		18,	L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.Characteristics)),
	};
}
