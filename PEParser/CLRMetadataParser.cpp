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

std::vector<ManagedMember> CLRMetadataParser::EnumMethods(mdToken token, bool includeInherited) const {
	std::vector<ManagedMember> members;
	EnumMethodsInternal(token, members);
	return members;
}

std::vector<ManagedMember> CLRMetadataParser::EnumFields(mdToken token, bool includeInherited) const {
	std::vector<ManagedMember> members;
	EnumFieldsInternal(token, members);
	return members;
}

std::vector<ManagedMember> CLRMetadataParser::EnumProperties(mdToken token, bool includeInherited) const {
	std::vector<ManagedMember> members;
	EnumPropertiesInternal(token, members);
	return members;
}

std::vector<ManagedMember> CLRMetadataParser::EnumEvents(mdToken token, bool includeInherited) const {
	std::vector<ManagedMember> members;
	EnumEventsInternal(token, members);
	return members;
}

ManagedTypeKind CLRMetadataParser::GetTypeKind(const ManagedType& type) const {
	if ((_enum == 0 && type.BaseTypeName == L"System.Enum") || (_enum && type.BaseTypeToken == _enum)) {
		if (_enum == 0)
			_enum = type.BaseTypeToken;
		return ManagedTypeKind::Enum;
	}
	if ((_delegate == 0 && type.BaseTypeName == L"System.MulticastDelegate") || (_delegate && type.BaseTypeToken == _delegate)) {
		if (_delegate == 0)
			_delegate = type.BaseTypeToken;
		return ManagedTypeKind::Delegate;
	}
	if ((_struct == 0 && type.BaseTypeName == L"System.ValueType") || (_struct && type.BaseTypeToken == _struct)) {
		if (_struct == 0)
			_struct = type.BaseTypeToken;
		return ManagedTypeKind::Struct;
	}
	if ((_attribute == 0 && type.BaseTypeName == L"System.Attribute") || (_attribute && type.BaseTypeToken == _attribute)) {
		if (_attribute == 0)
			_attribute = type.BaseTypeToken;
		return ManagedTypeKind::Attribute;
	}
	return ((type.Attributes & tdClassSemanticsMask) == tdInterface) ? ManagedTypeKind::Interface : ManagedTypeKind::Class;
}

void CLRMetadataParser::EnumMethodsInternal(mdToken token, std::vector<ManagedMember>& members, bool includeInherited) const {
	HCORENUM hEnum = nullptr;
	mdTypeDef defs[512];
	ULONG count;
	WCHAR name[256];
	ULONG len;
	PCCOR_SIGNATURE sig;
	ULONG sigSize;
	if (S_OK == _spImport->EnumMethods(&hEnum, token, defs, _countof(defs), &count)) {
		members.reserve(count + members.size());
		for (ULONG i = 0; i < count; i++) {
			ManagedMember member;
			_spImport->GetMethodProps(defs[i], &member.ClassToken, name, _countof(name), &len, &member.Attributes, &sig, &sigSize, &member.Method.CodeRva, &member.Method.ImplFlags);
			member.Name = name;
			member.Token = defs[i];
			if (member.Name == COR_CTOR_METHOD_NAME_W)
				member.Type = ManagedMemberType::Constructor;
			else if (member.Name == COR_CCTOR_METHOD_NAME_W)
				member.Type = ManagedMemberType::StaticConstructor;
			else
				member.Type = ManagedMemberType::Method;
			members.emplace_back(std::move(member));
		}
		_spImport->CloseEnum(hEnum);
		if (includeInherited) {
			mdToken baseToken = 0;
			_spImport->GetTypeDefProps(token, nullptr, 0, nullptr, nullptr, &baseToken);
			if (baseToken && _refTypesMap.find(baseToken) == _refTypesMap.end())
				EnumMethodsInternal(baseToken, members, includeInherited);
		}
	}
}

void CLRMetadataParser::EnumFieldsInternal(mdToken token, std::vector<ManagedMember>& members, bool includeInherited) const {
	HCORENUM hEnum = nullptr;
	mdTypeDef defs[256];
	ULONG count;
	WCHAR name[256];
	ULONG len;
	PCCOR_SIGNATURE sig;
	ULONG sigSize;
	UVCP_CONSTANT str;
	ULONG strSize;
	if (S_OK == _spImport->EnumFields(&hEnum, token, defs, _countof(defs), &count)) {
		members.reserve(count + members.size());
		for (ULONG i = 0; i < count; i++) {
			ManagedMember member;
			_spImport->GetFieldProps(defs[i], &member.ClassToken, name, _countof(name), &len, &member.Attributes, &sig, &sigSize, &member.Field.TypeFlags, &str, &strSize);
			member.Name = name;
			member.Token = defs[i];
			member.Type = ManagedMemberType::Field;
			members.emplace_back(std::move(member));
		}
		_spImport->CloseEnum(hEnum);
		if (includeInherited) {
			mdToken baseToken = 0;
			_spImport->GetTypeDefProps(token, nullptr, 0, nullptr, nullptr, &baseToken);
			if (baseToken && _refTypesMap.find(baseToken) == _refTypesMap.end())
				EnumFieldsInternal(baseToken, members, includeInherited);
		}
	}
}

void CLRMetadataParser::EnumPropertiesInternal(mdToken token, std::vector<ManagedMember>& members, bool includeInherited) const {
	HCORENUM hEnum = nullptr;
	mdTypeDef defs[256];
	ULONG count;
	WCHAR name[256];
	ULONG len;
	PCCOR_SIGNATURE sig;
	ULONG sigSize;
	UVCP_CONSTANT str;
	ULONG strSize;
	if (S_OK == _spImport->EnumProperties(&hEnum, token, defs, _countof(defs), &count)) {
		members.reserve(count + members.size());
		mdMethodDef otherMethods[16];
		ULONG lenOtherMethods;
		for (ULONG i = 0; i < count; i++) {
			ManagedMember member;
			_spImport->GetPropertyProps(defs[i], &member.ClassToken, name, _countof(name), &len, &member.Attributes, &sig, &sigSize,
				&member.Property.CPlusTypeFlag, &str, &strSize,
				&member.Property.Setter, &member.Property.Getter, otherMethods, _countof(otherMethods), &lenOtherMethods);
			member.Name = name;
			member.Token = defs[i];
			member.Type = ManagedMemberType::Property;
			members.emplace_back(std::move(member));
		}
		_spImport->CloseEnum(hEnum);
		if (includeInherited) {
			mdToken baseToken = 0;
			_spImport->GetTypeDefProps(token, nullptr, 0, nullptr, nullptr, &baseToken);
			if (baseToken && _refTypesMap.find(baseToken) == _refTypesMap.end())
				EnumPropertiesInternal(baseToken, members, includeInherited);
		}
	}
}

void CLRMetadataParser::EnumEventsInternal(mdToken token, std::vector<ManagedMember>& members, bool includeInherited) const {
	HCORENUM hEnum = nullptr;
	mdTypeDef defs[256];
	ULONG count;
	WCHAR name[256];
	ULONG len;
	if (S_OK == _spImport->EnumEvents(&hEnum, token, defs, _countof(defs), &count)) {
		members.resize(members.size() + count);
		mdMethodDef otherMethods[8];
		ULONG lenOtherMethods;
		for (ULONG i = 0; i < count; i++) {
			ManagedMember member;
			_spImport->GetEventProps(defs[i], &member.ClassToken, name, _countof(name), &len, &member.Attributes,
				&member.Event.EventType, &member.Event.AddMethod, &member.Event.RemoveMethod, &member.Event.FireMethod,
				otherMethods, _countof(otherMethods), &lenOtherMethods);
			member.Name = name;
			member.Token = defs[i];
			member.Type = ManagedMemberType::Event;
			members.emplace_back(std::move(member));
		}
		_spImport->CloseEnum(hEnum);
		if (includeInherited) {
			mdToken baseToken = 0;
			_spImport->GetTypeDefProps(token, nullptr, 0, nullptr, nullptr, &baseToken);
			if (baseToken && _refTypesMap.find(baseToken) == _refTypesMap.end())
				EnumEventsInternal(baseToken, members, includeInherited);
		}
	}
}

std::vector<ManagedMember> CLRMetadataParser::EnumMembers(mdToken token, bool includeInherited) const {
	std::vector<ManagedMember> members;
	members.reserve(32);
	EnumFieldsInternal(token, members, includeInherited);
	EnumMethodsInternal(token, members, includeInherited);
	EnumPropertiesInternal(token, members, includeInherited);
	EnumEventsInternal(token, members, includeInherited);

	return members;
}

CString CLRMetadataParser::GetMethodName(mdMethodDef token) const {
	WCHAR name[256];
	ULONG len;
	_spImport->GetMethodProps(token, nullptr, name, _countof(name), &len, nullptr, nullptr, nullptr, nullptr, nullptr);
	return name;
}

CString CLRMetadataParser::GetTypeName(mdToken token) const {
	WCHAR name[512];
	DWORD len;
	auto hr = _spImport->GetTypeDefProps(token, name, _countof(name), &len, nullptr, nullptr);
	if (SUCCEEDED(hr))
		return name;

	return L"";
}
