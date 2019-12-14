#pragma once

#include "PEParser.h"

class CLRMetadataParser {
public:
	CLRMetadataParser(IMetaDataImport* md);

	std::vector<ManagedType> EnumTypes() const;
	ManagedTypeKind GetTypeKind(const ManagedType& type) const;

private:
	mutable mdToken _delegate{ 0 }, _enum{ 0 }, _struct{ 0 }, _attribute{ 0 };
	CComPtr<IMetaDataImport> _spImport;
};

