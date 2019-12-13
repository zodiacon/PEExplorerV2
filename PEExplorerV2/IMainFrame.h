#pragma once

#include "PEParser.h"

enum class TreeNodeType {
	Root,
	Summary,
	Sections,
	Directories,
	Exports,
	Imports,
	Resources,
	Headers,
	ImportAddressTable,
	ThreadLocalStorage,
	LoadConfig,
	Debug,

	ImageDosHeader = 0x100,
	ImageFileHeader,
	ImageOptionalHeader,

	SectionView = 0x200,

	DirectoryView = 0x300,

	ExportView = 0x400,
};

struct IMainFrame {
	virtual UINT ShowContextMenu(HMENU menu, const POINT& pt, DWORD flags = 0) = 0;
	virtual CTreeItem CreateHexView(TreeNodeType type, PCWSTR title, LPARAM param) = 0;
	virtual CTreeItem CreateAssemblyView(const ExportedSymbol& symbol) = 0;
};
