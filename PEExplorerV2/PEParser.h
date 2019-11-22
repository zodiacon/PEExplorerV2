#pragma once

struct ExportedSymbol {
	std::string Name;
	std::string UndecoratedName;
	unsigned short Ordinal;
};

struct ImportedSymbol {
	std::string Name;
	std::string UndecoratedName;
	unsigned short Hint;
};

struct ImportedLibrary {
	std::string Name;
	std::vector<ImportedSymbol> Symbols;
};

enum class SubsystemType : short {
	Unknown,
	Native,
	WindowsGUI,
	WindowsCUI,
	PosixCUI = 7,
	WindowsCEGUI = 9,
	EfiApplication = 10,
	EfiBootServiceDriver = 11,
	EfiRuntimeDriver = 12,
	EfiROM = 13,
	XBOX = 14
};

enum class DllCharacteristics : unsigned short {
	None = 0,
	HighEntropyVA = 0x20,
	DynamicBase = 0x40,
	ForceIntegrity = 0x80,
	NxCompat = 0x100,
	NoIsolation = 0x200,
	NoSEH = 0x400,
	NoBind = 0x800,
	AppContainer = 0x1000,
	WDMDriver = 0x2000,
	ControlFlowGuard = 0x4000,
	TerminalServerAware = 0x8000
};
DEFINE_ENUM_FLAG_OPERATORS(DllCharacteristics);

enum class ImageCharacteristics : unsigned short {
	None = 0,
	RelocsStripped = 1,
	ExecutableImage = 2,
	LineNumsStripped = 4,
	LocalSymbolsStripped = 8,
	AggressiveTrimWorkingSet = 0x10,
	LargeAddressAware = 0x20,
	LittleEndian = 0x80,
	Machine32Bit = 0x100,
	DebugInfoStripped = 0x200,
	RemovableRunFromSwap = 0x400,
	NetRunFromSwap = 0x800,
	SystemFile = 0x1000,
	DllFile = 0x2000,
	SingleCpuOnly = 0x4000,
	BigEndian = 0x8000
};
DEFINE_ENUM_FLAG_OPERATORS(ImageCharacteristics);

enum class ImageDebugType {
	Unknown,
	Coff,
	CodeView,
	Fpo,
	Misc,
	Exception,
	Fixup,
	Borland = 9,
	Repro = 16
};

enum class ControlFlowGuardFlags {
	Instrumented = 0x100,
	WriteInstrumented = 0x200,
	FunctionTablePresent = 0x400,
	SecurityCookieUnused = 0x800,
	ProtectDelayLoadIAT = 0x1000,
	DelayLoadIATOwnSection = 0x2000,
	ExportSuppressInfo = 0x4000,
	EnableExportSuppression = 0x8000,
	LongJumpTablePresent = 0x10000
};

enum class SectionFlags : unsigned {
	NoPad = 8,
	Code = 0x20,
	InitializedData = 0x40,
	UninitializedData = 0x80,
	Other = 0x100,
	Info = 0x200,
	Remove = 0x800,
	Comdat = 0x1000,
	GPRel = 0x80000,
	Align1Byte = 0x100000,
	Align2Bytes = 0x200000,
	ExtendedReloc = 0x1000000,
	Discardable = 0x2000000,
	NotCached = 0x4000000,
	NotPaged = 0x8000000,
	Shared = 0x10000000,
	Execute = 0x20000000,
	Read = 0x40000000,
	Write = 0x80000000,
};

enum class OptionalHeaderMagic : short {
	PE32 = 0x10b,
	PE32Plus = 0x20b,
	Rom = 0x107
};

enum class MachineType : unsigned short {
	Unknown = 0,
	X86 = 332,
	Arm = 0x1c0,
	Arm_NT = 0x1c4,
	IA64 = 512,
	Amd64 = 34404,
	Arm64 = 43620,
};

class PEParser final {
public:
	PEParser(const char* path);
	~PEParser();

	bool IsValid() const;
	bool IsPe64() const;
	bool IsExecutable() const;

	int GetSectionCount() const;
	const IMAGE_SECTION_HEADER* GetSectionHeader(ULONG section) const;
	const IMAGE_DATA_DIRECTORY* GetDataDirectory(int index) const;

	std::vector<ExportedSymbol> GetExports() const;
	std::vector<ImportedLibrary> GetImports() const;
	const IMAGE_FILE_HEADER& GetFileHeader() const;

	void* GetAddress(unsigned rva) const;

	SubsystemType GetSubsystemType() const;
	CString GetFileName() const;

	IMAGE_OPTIONAL_HEADER32& GetOptionalHeader32() const {
		return *(IMAGE_OPTIONAL_HEADER32*)&_image->FileHeader->OptionalHeader;
	}
	IMAGE_OPTIONAL_HEADER64& GetOptionalHeader64() const {
		return _image->FileHeader->OptionalHeader;
	}

private:
	PLOADED_IMAGE _image;
};


