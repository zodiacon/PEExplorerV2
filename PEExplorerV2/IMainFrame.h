#pragma once

enum class TreeNodeType {
	Root,
	Summary,
	Sections,
	Directories,
	Exports,
	Imports,
	Resources,
	ImportAddressTable,
	ThreadLocalStorage,
	LoadConfig,
	Debug,

	ImageDosHeader = 0x100,
	ImageFileHeader,
	OptionalHeader,

	SectionView = 0x200,
	SectionViewLast = 0x2ff,

	DirectoryView = 0x300,
	DirectoryViewLast = 0x30f,
};

struct IMainFrame {
	virtual UINT ShowContextMenu(HMENU menu, const POINT& pt, DWORD flags = 0) = 0;
	virtual CTreeItem CreateHexView(TreeNodeType type, PCWSTR title, LPARAM param) = 0;
};
