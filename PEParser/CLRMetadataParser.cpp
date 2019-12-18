#include "pch.h"
#include "CLRMetadataParser.h"

CLRMetadataParser::CLRMetadataParser(IMetaDataImport* md) : _spImport(md) {
	ATLASSERT(md);
}

std::vector<ManagedType> CLRMetadataParser::EnumTypes() const {
	std::vector<ManagedType> types;

	HCORENUM hEnum = nullptr;
	mdTypeDef typedefs[2048];
	ULONG count;
	auto hr = _spImport->EnumTypeDefs(&hEnum, typedefs, _countof(typedefs), &count);
	WCHAR name[1024];
	ULONG len;
	DWORD flags;
	mdToken baseType;
	types.reserve(count);
	for (decltype(count) i = 0; i < count; i++) {
		hr = _spImport->GetTypeDefProps(typedefs[i], name, _countof(name), &len, &flags, &baseType);
		if (FAILED(hr))
			continue;
		ManagedType type;
		type.Name = name;
		type.Token = typedefs[i];
		type.Attributes = (CorTypeAttr)flags;
		type.BaseTypeToken = baseType;

		hr = _spImport->GetTypeDefProps(baseType, name, _countof(name), &len, &flags, &baseType);
		if (hr == S_OK)
			type.BaseTypeName = name;
		else if (auto it = _refTypesMap.find(type.BaseTypeToken); it != _refTypesMap.end())
			type.BaseTypeName = it->second.Name;

		type.Kind = GetTypeKind(type);
		types.emplace_back(std::move(type));
	}
	_spImport->CloseEnum(hEnum);
	return types;
}

std::vector<ManagedTypeRef> CLRMetadataParser::EnumRefTypes() const {
	if (!_refTypes.empty())
		return _refTypes;

	std::vector<ManagedTypeRef> types;

	HCORENUM hEnum = nullptr;
	mdTypeRef typedefs[2048];
	ULONG count;
	auto hr = _spImport->EnumTypeRefs(&hEnum, typedefs, _countof(typedefs), &count);
	WCHAR name[1024];
	ULONG len;
	types.reserve(count);
	_refTypesMap.reserve(count);
	mdToken resScope;
	for (decltype(count) i = 0; i < count; i++) {
		hr = _spImport->GetTypeRefProps(typedefs[i], &resScope, name, _countof(name), &len);
		if (FAILED(hr))
			continue;
		ManagedTypeRef type;
		type.Name = name;
		type.Token = typedefs[i];
		type.Scope = resScope;
		_refTypesMap.insert({ type.Token, type });
		types.emplace_back(std::move(type));
	}
	_spImport->CloseEnum(hEnum);
	_refTypes = std::move(types);
	return _refTypes;
}

std::vector<ManagedMember> CLRMetadataParser::EnumMethods(mdToken token) const {
	std::vector<ManagedMember> members;
	HCORENUM hEnum = nullptr;
	mdTypeDef defs[512];
	ULONG count;
	WCHAR name[256];
	ULONG len;
	DWORD attr, implFlags;
	PCCOR_SIGNATURE sig;
	ULONG sigSize;
	ULONG codeRva;
	if (S_OK == _spImport->EnumMethods(&hEnum, token, defs, _countof(defs), &count)) {
		members.reserve(count);
		mdToken tkClass;
		for (ULONG i = 0; i < count; i++) {
			_spImport->GetMethodProps(defs[i], &tkClass, name, _countof(name), &len, &attr, &sig, &sigSize, &codeRva, &implFlags);
			ManagedMember member;
			member.Name = name;
			member.Token = defs[i];
			member.Type = ManagedMemberType::Method;
			member.Attributes = attr;
			member.CodeRva = codeRva;
			member.OtherFlags = implFlags;
			member.ClassToken = tkClass;
			members.emplace_back(std::move(member));
		}
	}
	return members;
}

std::vector<ManagedMember> CLRMetadataParser::EnumFields(mdToken token) const {
	std::vector<ManagedMember> members;
	HCORENUM hEnum = nullptr;
	mdTypeDef defs[256];
	ULONG count;
	WCHAR name[256];
	ULONG len;
	DWORD attr, typeFlags;
	PCCOR_SIGNATURE sig;
	ULONG sigSize;
	UVCP_CONSTANT str;
	ULONG strSize;
	if (S_OK == _spImport->EnumFields(&hEnum, token, defs, _countof(defs), &count)) {
		members.reserve(count);
		mdToken tkClass;
		for (ULONG i = 0; i < count; i++) {
			_spImport->GetFieldProps(defs[i], &tkClass, name, _countof(name), &len, &attr, &sig, &sigSize, &typeFlags, &str, &strSize);
			ManagedMember member;
			member.Name = name;
			member.Token = defs[i];
			member.Type = ManagedMemberType::Method;
			member.Attributes = attr;
			member.OtherFlags = typeFlags;
			member.ClassToken = tkClass;
			members.emplace_back(std::move(member));
		}
	}
	return members;
}

std::vector<ManagedMember> CLRMetadataParser::EnumProperties(mdToken token) const {
	std::vector<ManagedMember> members;
	HCORENUM hEnum = nullptr;
	mdTypeDef defs[256];
	ULONG count;
	WCHAR name[256];
	ULONG len;
	DWORD attr, typeFlags;
	PCCOR_SIGNATURE sig;
	ULONG sigSize;
	UVCP_CONSTANT str;
	ULONG strSize;
	if (S_OK == _spImport->EnumProperties(&hEnum, token, defs, _countof(defs), &count)) {
		members.reserve(count);
		mdToken tkClass, setter, getter;
		mdMethodDef otherMethods[16];
		ULONG lenOtherMethods;
		for (ULONG i = 0; i < count; i++) {
			_spImport->GetPropertyProps(defs[i], &tkClass, name, _countof(name), &len, &attr, &sig, &sigSize, &typeFlags, &str, &strSize,
				&setter, &getter, otherMethods, _countof(otherMethods), &lenOtherMethods);
			ManagedMember member;
			member.Name = name;
			member.Token = defs[i];
			member.Type = ManagedMemberType::Property;
			member.Attributes = attr;
			member.OtherFlags = typeFlags;
			member.ClassToken = tkClass;
			members.emplace_back(std::move(member));
		}
	}
	return members;
}


ManagedTypeKind CLRMetadataParser::GetTypeKind(const ManagedType& type) const {
	if ((_enum == 0 && type.BaseTypeName == L"System.Enum") || (_enum && type.BaseTypeToken == _enum)) {
		if(_enum == 0)
			_enum = type.BaseTypeToken;
		return ManagedTypeKind::Enum;
	}
	if ((_delegate == 0 && type.BaseTypeName == L"System.MulticastDelegate") || (_delegate && type.BaseTypeToken == _delegate)) {
		if(_delegate == 0)
			_delegate = type.BaseTypeToken;
		return ManagedTypeKind::Delegate;
	}
	if ((_struct == 0 && type.BaseTypeName == L"System.ValueType") || (_struct && type.BaseTypeToken == _struct)) {
		if(_struct == 0)
			_struct = type.BaseTypeToken;
		return ManagedTypeKind::Struct;
	}
	if ((_attribute == 0 && type.BaseTypeName == L"System.Attribute") || (_attribute && type.BaseTypeToken == _attribute)) {
		if(_attribute == 0)
			_attribute = type.BaseTypeToken;
		return ManagedTypeKind::Attribute;
	}
	return ((type.Attributes & tdClassSemanticsMask) == tdInterface) ? ManagedTypeKind::Interface : ManagedTypeKind::Class;
}
