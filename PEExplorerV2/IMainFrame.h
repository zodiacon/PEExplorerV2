#pragma once

#include "PEParser.h"

enum class TreeNodeType : size_t {
	Root,
	Summary,
	Sections,
	Directories,
	Exports,
	Imports,
	Resources,
	DotNet,
	Headers,
	ImportAddressTable,
	ThreadLocalStorage,
	LoadConfig,
	Debug,

	ImageDosHeader = 0x100,
	ImageFileHeader,
	ImageOptionalHeader,

	SectionView = 0x10000,

	DirectoryView = 0x11000,

	ExportView = 0x12000,

	ManagedTypeMembersView = 0x13000
};

struct IMainFrame {
	virtual UINT ShowContextMenu(HMENU menu, const POINT& pt, DWORD flags = 0) = 0;
	virtual CTreeItem CreateHexView(TreeNodeType type, PCWSTR title, LPARAM param) = 0;
	virtual CTreeItem CreateAssemblyView(const ExportedSymbol& symbol) = 0;
	virtual bool OpenDocument(PCWSTR name, bool newWindow) = 0;
	virtual CTreeItem CreateTypeMembersView(const ManagedType& type) = 0;
};
