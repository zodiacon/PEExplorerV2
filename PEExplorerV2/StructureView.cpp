#include "pch.h"
#include "StructureView.h"

const char* dosHeader =
	"WORD   e_magic;     Magic number\n"
	"WORD   e_cblp;      Bytes on last page of file\n"
	"WORD   e_cp;        Pages in file\n"
	"WORD   e_crlc;      Relocations\n"
	"WORD   e_cparhdr;   Size of header in paragraphs\n"
	"WORD   e_minalloc;  Minimum extra paragraphs needed\n"
	"WORD   e_maxalloc;  Maximum extra paragraphs needed\n"
	"WORD   e_ss;        Initial (relative) SS value\n"
	"WORD   e_sp;        Initial SP value\n"
	"WORD   e_csum;      Checksum\n"
	"WORD   e_ip;        Initial IP value\n"
	"WORD   e_cs;        Initial (relative) CS value\n"
	"WORD   e_lfarlc;    File address of relocation table\n"
	"WORD   e_ovno;      Overlay number\n"
	"WORD   e_res[4];    Reserved words\n"
	"WORD   e_oemid;     OEM identifier (for e_oeminfo)\n"
	"WORD   e_oeminfo;   OEM information; e_oemid specific\n"
	"WORD   e_res2[10];  Reserved words\n"
	"LONG   e_lfanew;    File address of new exe header\n";

StructureView::StructureView(PEParser* parser, StructureType type) : _parser(parser), _type(type) {
}

void StructureView::Init(CListViewCtrl& lv) {
	lv.InsertColumn(0, L"Name", LVCFMT_LEFT, 150);
	lv.InsertColumn(1, L"Offset", LVCFMT_RIGHT, 100);
	lv.InsertColumn(2, L"Type", LVCFMT_LEFT, 100);
	lv.InsertColumn(3, L"Value", LVCFMT_RIGHT, 150);
}

int StructureView::GetItemCount() {
	return 0;
}

CString StructureView::GetItemText(int row, int col) {
	return CString();
}

bool StructureView::Sort(int column, bool ascending) {
	return false;
}
