#pragma once

#include "StructureView.h"
#include "PEParser.h"

class ImageFileHeaderStruct : public IStructureProvider {
public:
	ImageFileHeaderStruct(PEParser* parser);

	// IStructureProvider
	CString GetName() const override;
	int GetMemberCount() const override;
	const MemberInfo& GetMemberInfo(int index) const override;

private:
	void BuildMembers();

private:
	PEParser* _parser;
	std::vector<MemberInfo> _members;
};

