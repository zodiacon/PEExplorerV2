#include "pch.h"
#include "ImageDosHeaderStruct.h"
#include "PEStrings.h"

ImageDosHeaderStruct::ImageDosHeaderStruct(PEParser* parser) : _parser(parser) {
	BuildMembers();
}

CString ImageDosHeaderStruct::GetName() const {
	return L"IMAGE_DOS_HEADER";
}

int ImageDosHeaderStruct::GetMemberCount() const {
	return static_cast<int>(_members.size());
}

const MemberInfo& ImageDosHeaderStruct::GetMemberInfo(int index) const {
	return _members[index];
}

void ImageDosHeaderStruct::BuildMembers() {
	auto& header = _parser->GetDosHeader();

	_members = std::vector<MemberInfo>{
		MemberInfo(L"e_magic",		 0,	L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_magic), L"Magic ('MZ')"),
		MemberInfo(L"e_cblp",		 2,	L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_cblp)),
		MemberInfo(L"e_cp",			 4,	L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_cp)),
		MemberInfo(L"e_crlc",		 6, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_crlc)),
		MemberInfo(L"e_cparhdr",	 8, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_cparhdr)),
		MemberInfo(L"e_minalloc",	10, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_minalloc)),
		MemberInfo(L"e_maxalloc",	12, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_maxalloc)),
		MemberInfo(L"e_ss",			14, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_ss)),
		MemberInfo(L"e_sp",			16, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_sp)),
		MemberInfo(L"e_csum",		18, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_csum)),
		MemberInfo(L"e_ip",			20, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_ip)),
		MemberInfo(L"e_cs",			22, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_cs)),
		MemberInfo(L"e_lfarlc",		24, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_lfarlc)),
		MemberInfo(L"e_ovno",		26, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_ovno)),
		MemberInfo(L"e_res[4]",		28, L"WORD", sizeof(WORD[4]), L""),
		MemberInfo(L"e_oemid",		36, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_oemid)),
		MemberInfo(L"e_oeminfo",	38, L"WORD", sizeof(WORD), PEStrings::ToDecAndHex(header.e_oeminfo)),
		MemberInfo(L"e_res2[10]",	40, L"WORD", sizeof(WORD[10]), L""),
		MemberInfo(L"e_lfanew",		60, L"LONG", sizeof(LONG), PEStrings::ToDecAndHex(header.e_lfanew), L"Offset to Image file header"),
	};
}
