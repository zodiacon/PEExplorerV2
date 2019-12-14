#include "pch.h"
#include "CLRMetadataParser.h"

CLRMetadataParser::CLRMetadataParser(IMetaDataImport* md) : _spImport(md) {
	ATLASSERT(md);
}

std::vector<ManagedType> CLRMetadataParser::EnumTypes() const {
	std::vector<ManagedType> types;

	HCORENUM hEnum = nullptr;
	mdTypeDef typedefs[4096];
	ULONG count;
	auto hr = _spImport->EnumTypeDefs(&hEnum, typedefs, _countof(typedefs), &count);
	WCHAR name[2048];
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
		type.Kind = GetTypeKind(type);
		types.emplace_back(std::move(type));
	}
	_spImport->CloseEnum(hEnum);
	return types;
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
