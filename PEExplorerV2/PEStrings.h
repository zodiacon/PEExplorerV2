#pragma once

#include "PEParser.h"
#include <atltime.h>
#include "Capstone/capstone.h"

struct PEStrings abstract final {
	static PCWSTR SubsystemTypeToString(SubsystemType type);
	static CString GetFileName(const CString& path);
	static CString ToDecAndHex(DWORD value, bool hexFirst = false);
	static CString MagicToString(OptionalHeaderMagic magic);
	static CString MachineTypeToString(MachineType type);
	static CString Sec1970ToString(DWORD secs);
	static CString CharacteristicsToString(ImageCharacteristics ch);
	static CString DllCharacteristicsToString(DllCharacteristics ch);
	static CString ToHex(DWORD value, bool leadingZero = false);
	static CString ToHex(ULONGLONG value);
	static CString ToMemorySize(ULONGLONG size);
	static PCWSTR ResourceTypeToString(WORD id);
	static CString FormatInstruction(cs_insn& inst);
	static CString ManagedTypeAttributesToString(CorTypeAttr attr);
};

