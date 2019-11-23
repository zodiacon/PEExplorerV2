#include "pch.h"
#include "PEStrings.h"

PCWSTR PEStrings::SubsystemTypeToString(SubsystemType type) {
	switch (type) {
		case SubsystemType::Native: return L"Native";
		case SubsystemType::WindowsGUI: return L"Windows GUI";
		case SubsystemType::WindowsCUI: return L"Windows CUI";
		case SubsystemType::PosixCUI: return L"Posix CUI";
		case SubsystemType::WindowsCEGUI: return L"Windows CE GUI";
		case SubsystemType::EfiApplication: return L"EFI Application";
		case SubsystemType::EfiBootServiceDriver: return L"EFI Boot Service Driver";
		case SubsystemType::EfiRuntimeDriver: return L"EFI Runtime Driver";
		case SubsystemType::EfiROM: return L"EFI ROM";
		case SubsystemType::XBOX: return L"XBOX";
	}
	return L"Unknown";
}

CString PEStrings::GetFileName(const CString& path) {
	auto dot = path.ReverseFind(L'\\');
	if (dot < 0)
		return path;
	return path.Mid(dot + 1);
}

CString PEStrings::ToDecAndHex(DWORD value, bool hexFirst) {
	CString text;
	if (hexFirst)
		text.Format(L"0x%X (%u)", value, value);
	else
		text.Format(L"%u (0x%X)", value, value);
	return text;
}

CString PEStrings::MagicToString(OptionalHeaderMagic magic) {
	switch (magic) {
		case OptionalHeaderMagic::PE32: return L"PE32";
		case OptionalHeaderMagic::PE32Plus: return L"PE32+";
		case OptionalHeaderMagic::Rom: return L"ROM";
	}
	return L"";
}

CString PEStrings::MachineTypeToString(MachineType type) {
	switch (type) {
		case MachineType::Unknown: return L"Unknown";
		case MachineType::X86: return L"x86";
		case MachineType::Arm: return L"ARM";
		case MachineType::Arm_NT: return L"ARM NT";
		case MachineType::IA64: return L"IA64";
		case MachineType::Amd64: return L"AMD64 (x64)";
		case MachineType::Arm64: return L"ARM64";
	}
	return L"";
}

CString PEStrings::Sec1970ToString(DWORD secs) {
	return CTime(secs).Format(L"%X");
}

CString PEStrings::CharacteristicsToString(ImageCharacteristics ch) {
	CString result;

	if ((ch & ImageCharacteristics::RelocsStripped) != ImageCharacteristics::None)
		result += L"Relocations Stripped (1), ";
	if ((ch & ImageCharacteristics::ExecutableImage) != ImageCharacteristics::None)
		result += L"Executable Image (2), ";
	if ((ch & ImageCharacteristics::LineNumsStripped) != ImageCharacteristics::None)
		result += L"Line Numbers Stripped (4), ";
	if ((ch & ImageCharacteristics::LocalSymbolsStripped) != ImageCharacteristics::None)
		result += L"Local Symbols Stripped (8), ";
	if ((ch & ImageCharacteristics::DllFile) != ImageCharacteristics::None)
		result += L"Dll File (0x2000), ";
	if ((ch & ImageCharacteristics::AggressiveTrimWorkingSet) != ImageCharacteristics::None)
		result += L"Aggressive Trim Working Set (0x10), ";
	if ((ch & ImageCharacteristics::LargeAddressAware) != ImageCharacteristics::None)
		result += L"Large Address Aware (0x20), ";
	if ((ch & ImageCharacteristics::LittleEndian) != ImageCharacteristics::None)
		result += L"Little Endian (0x80), ";
	if ((ch & ImageCharacteristics::Machine32Bit) != ImageCharacteristics::None)
		result += L"32 bit Machine (0x100), ";
	if ((ch & ImageCharacteristics::DebugInfoStripped) != ImageCharacteristics::None)
		result += L"Debug Info Stripped (0x200), ";
	if ((ch & ImageCharacteristics::RemovableRunFromSwap) != ImageCharacteristics::None)
		result += L"Removable Run from Swap (0x400), ";
	if ((ch & ImageCharacteristics::NetRunFromSwap) != ImageCharacteristics::None)
		result += L"Net Run from Swap (0x800), ";
	if ((ch & ImageCharacteristics::SingleCpuOnly) != ImageCharacteristics::None)
		result += L"Single CPU Only (0x4000), ";
	if ((ch & ImageCharacteristics::BigEndian) != ImageCharacteristics::None)
		result += L"Big Endian (0x8000), ";

	if (!result.IsEmpty())
		result = result.Left(result.GetLength() - 2);
	return result;
}

CString PEStrings::DllCharacteristicsToString(DllCharacteristics ch) {
	CString result;
	if ((ch & DllCharacteristics::HighEntropyVA) != DllCharacteristics::None)
		result += L"High Entropy VA (0x20), ";
	if ((ch & DllCharacteristics::DynamicBase) != DllCharacteristics::None)
		result += L"Dynamic Base (0x40), ";
	if ((ch & DllCharacteristics::ForceIntegrity) != DllCharacteristics::None)
		result += L"Force Integrity (0x80), ";
	if ((ch & DllCharacteristics::NxCompat) != DllCharacteristics::None)
		result += L"NX Compat (0x100), ";
	if ((ch & DllCharacteristics::NoIsolation) != DllCharacteristics::None)
		result += L"No Isolation (0x200), ";
	if ((ch & DllCharacteristics::NoSEH) != DllCharacteristics::None)
		result += L"No SEH (0x400), ";
	if ((ch & DllCharacteristics::NoBind) != DllCharacteristics::None)
		result += L"No Bind (0x800), ";
	if ((ch & DllCharacteristics::AppContainer) != DllCharacteristics::None)
		result += L"App Container (0x1000), ";
	if ((ch & DllCharacteristics::WDMDriver) != DllCharacteristics::None)
		result += L"WDM Driver (0x2000), ";
	if ((ch & DllCharacteristics::ControlFlowGuard) != DllCharacteristics::None)
		result += L"Control Flow Guard (0x4000), ";
	if ((ch & DllCharacteristics::TerminalServerAware) != DllCharacteristics::None)
		result += L"Terminal Server Aware (0x8000), ";

	if (!result.IsEmpty())
		result = result.Left(result.GetLength() - 2);
	return result;

}

CString PEStrings::ToHex(DWORD value, bool leadingZero) {
	CString result;
	result.Format(leadingZero ? L"0x%08X" : L"0x%X", value);
	return result;
}

CString PEStrings::ToHex(ULONGLONG value) {
	CString result;
	result.Format(L"0x%llX", value);
	return result;
}

CString PEStrings::ToMemorySize(ULONGLONG size) {
	CString result;
	result.Format(L"0x%llX", size);
	if (size < 1 << 14)
		result.Format(L"%s (%u B)", (PCWSTR)result, size);
	else if (size < 1 << 23)
		result.Format(L"%s (%u KB)", (PCWSTR)result, size >> 10);
	else if (size < 1LL << 33)
		result.Format(L"%s (%u MB)", (PCWSTR)result, size >> 20);
	else
		result.Format(L"%s (%u GB)", (PCWSTR)result, size >> 30);
	return result;
}


