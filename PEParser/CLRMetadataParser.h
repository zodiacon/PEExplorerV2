#pragma once

#include "PEParser.h"
#include <unordered_map>

class CLRMetadataParser final {
public:
	CLRMetadataParser(IMetaDataImport* md);

	std::vector<ManagedType> EnumTypes() const;
	std::vector<ManagedTypeRef> EnumRefTypes() const;
	std::vector<ManagedMember> EnumMembers(mdToken token, bool includeInherited = false) const;
	std::vector<ManagedMember> EnumMethods(mdToken token, bool includeInherited = false) const;
	std::vector<ManagedMember> EnumFields(mdToken token, bool includeInherited = false) const;
	std::vector<ManagedMember> EnumProperties(mdToken token, bool includeInherited = false) const;
	std::vector<ManagedMember> EnumEvents(mdToken token, bool includeInherited = false) const;
	ManagedTypeKind GetTypeKind(const ManagedType& type) const;
	CString GetMethodName(mdMethodDef token) const;
	CString GetTypeName(mdToken token) const;

private:
	void EnumMethodsInternal(mdToken token, std::vector<ManagedMember>& members, bool includeInherited = false) const;
	void EnumFieldsInternal(mdToken token, std::vector<ManagedMember>& members, bool includeInherited = false) const;
	void EnumPropertiesInternal(mdToken token, std::vector<ManagedMember>& members, bool includeInherited = false) const;
	void EnumEventsInternal(mdToken token, std::vector<ManagedMember>& members, bool includeInherited = false) const;

private:
	mutable mdToken _delegate{ 0 }, _enum{ 0 }, _struct{ 0 }, _attribute{ 0 };
	CComPtr<IMetaDataImport> _spImport;
	mutable std::vector<ManagedTypeRef> _refTypes;
	mutable std::unordered_map<mdToken, ManagedTypeRef> _refTypesMap;
};

