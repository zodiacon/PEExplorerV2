#include "pch.h"
#include "PEStrings.h"

static PCWSTR memberVisiblity[] = {
	L"Private Scope", L"Private", L"Protected and Internal", L"Internal", L"Protected",
	L"Protected or Internal", L"Public"
};

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
	return L"(Unknown)";
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

CString PEStrings::CharacteristicsToString(ImageCharacteristics ic) {
	CString result;

	static const struct {
		ImageCharacteristics cs;
		PCWSTR text;
	} chars[] = {
		{ ImageCharacteristics::RelocsStripped, L"Relocations Stripped" },
		{ ImageCharacteristics::ExecutableImage, L"Executable Image" },
		{ ImageCharacteristics::AggressiveTrimWorkingSet, L"Aggressive Trim Working Set" },
		{ ImageCharacteristics::BigEndian, L"Big Endian" },
		{ ImageCharacteristics::LargeAddressAware, L"Large Address Aware" },
		{ ImageCharacteristics::DebugInfoStripped, L"Debug Info Stripped" },
		{ ImageCharacteristics::LineNumsStripped, L"Line Numbers Stripped" },
		{ ImageCharacteristics::DllFile, L"DLL File" },
		{ ImageCharacteristics::LittleEndian, L"Little Endian" },
		{ ImageCharacteristics::Machine32Bit, L"32-Bit Machine" },
		{ ImageCharacteristics::LocalSymbolsStripped, L"Local Symbols Stripped" },
		{ ImageCharacteristics::NetRunFromSwap, L"Net Run from Swap" },
		{ ImageCharacteristics::RemovableRunFromSwap, L"Removable Run from Swap" },
		{ ImageCharacteristics::SingleCpuOnly, L"Single CPU Only" },
		{ ImageCharacteristics::SystemFile, L"System File" },
	};

	for (auto& ch : chars) {
		if ((ch.cs & ic) == ch.cs)
			result += CString(ch.text) + L", ";
	}

	if (!result.IsEmpty())
		result = result.Left(result.GetLength() - 2);
	return result;
}

CString PEStrings::DllCharacteristicsToString(DllCharacteristics dc) {
	CString result;

	static const struct {
		DllCharacteristics cs;
		PCWSTR text;
	} chars[] = {
		{ DllCharacteristics::AppContainer, L"App Container" },
		{ DllCharacteristics::HighEntropyVA, L"High Entropy VA" },
		{ DllCharacteristics::DynamicBase, L"Dynamic Base" },
		{ DllCharacteristics::ForceIntegrity, L"Force Integrity" },
		{ DllCharacteristics::NxCompat, L"NX Compat" },
		{ DllCharacteristics::ControlFlowGuard, L"Control Flow Guard" },
		{ DllCharacteristics::NoBind, L"No Bind" },
		{ DllCharacteristics::WDMDriver, L"WDM Driver" },
		{ DllCharacteristics::NoIsolation, L"No Isolation" },
		{ DllCharacteristics::TerminalServerAware, L"Terminal Server Aware" },
		{ DllCharacteristics::NoSEH, L"No SEH" },
	};

	for (auto& ch : chars) {
		if ((ch.cs & dc) == ch.cs)
			result += CString(ch.text) + L", ";
	}

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

PCWSTR PEStrings::ResourceTypeToString(WORD id) {
	static PCWSTR types[] = {
		nullptr, L"Cursor", L"Bitmap", L"Icon",	L"Menu", L"Dialog",
		L"String Table", L"Font Directory", L"Font",
		L"Accelerators", L"RC Data", L"Message Table", L"Group Cursor", nullptr,
		L"Group Icon", nullptr, L"Version", L"Dialog Include",
		nullptr, L"Plug & Play", L"VxD", L"Animated Cursor", L"Animated Icon",
		L"HTML", L"Manifest"
	};
	return id >= _countof(types) ? nullptr : types[id];
}

CString PEStrings::FormatInstruction(const cs_insn& inst) {
	CStringA text;
	text.Format("%llX %-10s %s", inst.address, inst.mnemonic, inst.op_str);
	return CString(text);
}

CString PEStrings::ManagedTypeAttributesToString(CorTypeAttr attr) {
	static PCWSTR visiblity[] = {
		L"Private", L"Public", L"Nested Public", L"Nested Private",
		L"Nested Family", L"Nested Internal", L"Nested Protected and Internal", L"Nested Protected Internal"
	};

	CString text = visiblity[attr & tdVisibilityMask];
	text += L", ";

	if ((attr & tdLayoutMask) == tdSequentialLayout)
		text += L"Sequential, ";
	else if ((attr & tdLayoutMask) == tdExplicitLayout)
		text += L"Explicit, ";

	if ((attr & tdClassSemanticsMask) == tdInterface)
		text += L"Interface, ";

	if(attr & tdAbstract)
		text += L"Abstract, ";
	if (attr & tdSealed)
		text += L"Sealed, ";
	if (attr & tdSpecialName)
		text += L"Special Name, ";
	if (attr & tdImport)
		text += L"Import, ";
	if (attr & tdSerializable)
		text += L"Serializable, ";
	if (attr & tdWindowsRuntime)
		text += L"Windows Runtime, ";

	return text.Left(text.GetLength() - 2);
}

CString PEStrings::MemberAttributesToString(const ManagedMember& member) {
	switch (member.Type) {
		case ManagedMemberType::Method:
		case ManagedMemberType::Constructor:
		case ManagedMemberType::StaticConstructor:
			return MethodAttributesToString((CorMethodAttr)member.Attributes);
		case ManagedMemberType::Field:
			return FieldAttributesToString((CorFieldAttr)member.Attributes);
		case ManagedMemberType::Property:
			return PropertyAttributesToString((CorPropertyAttr)member.Attributes);
		case ManagedMemberType::Event:
			return EventAttributesToString((CorEventAttr)member.Attributes);
	}

	return L"";
}

CString PEStrings::MethodAttributesToString(CorMethodAttr attr) {
	CString text = memberVisiblity[attr & mdMemberAccessMask];
	text += L", ";

	if (attr & mdStatic)
		text += L"Static, ";
	if (attr & mdFinal)
		text += "Final, ";
	if (attr & mdVirtual)
		text += "Virtual, ";
	if (attr & mdHideBySig)
		text += "Hide By Sig, ";

	if ((attr & mdVtableLayoutMask) == mdNewSlot)
		text += L"New Slot, ";

	if (attr & mdCheckAccessOnOverride)
		text += "Check Access on Override, ";
	if (attr & mdAbstract)
		text += "Abstract, ";
	if (attr & mdSpecialName)
		text += "Special Name, ";
	if (attr & mdPinvokeImpl)
		text += "P/Invoke, ";
	if (attr & mdUnmanagedExport)
		text += "Unmanaged Export, ";

	if ((attr & mdReservedMask) == mdRTSpecialName)
		text += L"Runtime Special Name, ";
	if ((attr & mdReservedMask) == mdHasSecurity)
		text += L"Has Security, ";
	if ((attr & mdReservedMask) == mdRequireSecObject)
		text += L"Require Secure Object, ";

	return text.Left(text.GetLength() - 2);
}

CString PEStrings::FieldAttributesToString(CorFieldAttr attr) {
	CString text = memberVisiblity[attr & fdFieldAccessMask];
	text += L", ";

	if (attr & fdStatic)
		text += L"Static, ";
	if (attr & fdInitOnly)
		text += L"Read Only, ";
	if (attr & fdLiteral)
		text += L"Literal, ";
	if (attr & fdNotSerialized)
		text += L"Not Serialized, ";
	if (attr & fdSpecialName)
		text += L"Special Name, ";
	if (attr & fdPinvokeImpl)
		text += L"P/Invoke, ";

	if ((attr & fdReservedMask) == fdRTSpecialName)
		text += L"Runtime Special Name, ";
	if ((attr & fdReservedMask) == fdHasFieldMarshal)
		text += L"Has Field Marshal, ";
	if ((attr & fdReservedMask) == fdHasDefault)
		text += L"Has Default, ";
	if ((attr & fdReservedMask) == fdHasFieldRVA)
		text += L"Has Field RVA, ";

	return text.Left(text.GetLength() - 2);
}

CString PEStrings::PropertyAttributesToString(CorPropertyAttr attr) {
	CString text;

	if (IsPrSpecialName(attr))
		text += L"Special Name, ";
	if (IsPrRTSpecialName(attr))
		text += L"Runtime Special Name";
	if (IsPrHasDefault(attr))
		text += L"Has Default, ";

	if (!text.IsEmpty())
		text = text.Left(text.GetLength() - 2);
	return text;
}

CString PEStrings::EventAttributesToString(CorEventAttr attr) {
	CString text;

	if (IsEvSpecialName(attr))
		text += L"Special Name, ";
	if (IsEvRTSpecialName(attr))
		text += L"Runtime Special Name";

	if (!text.IsEmpty())
		text = text.Left(text.GetLength() - 2);
	return text;
}


