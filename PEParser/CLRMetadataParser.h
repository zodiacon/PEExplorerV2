#pragma once

#include "PEParser.h"
#include <unordered_map>

class CLRMetadataParser {
public:
	CLRMetadataParser(IMetaDataImport* md);

	std::vector<ManagedType> EnumTypes() const;
	std::vector<ManagedTypeRef> EnumRefTypes() const;
	std::vector<ManagedMember> EnumMembers(mdToken token) const;
	std::vector<ManagedMember> EnumMethods(mdToken token) const;
	std::vector<ManagedMember> EnumFields(mdToken token) const;
	std::vector<ManagedMember> EnumProperties(mdToken token) const;
	std::vector<ManagedMember> EnumEvents(mdToken token) const;
	ManagedTypeKind GetTypeKind(const ManagedType& type) const;

private:
	mutable mdToken _delegate{ 0 }, _enum{ 0 }, _struct{ 0 }, _attribute{ 0 };
	CComPtr<IMetaDataImport> _spImport;
	mutable std::vector<ManagedTypeRef> _refTypes;
	mutable std::unordered_map<mdToken, ManagedTypeRef> _refTypesMap;
};

