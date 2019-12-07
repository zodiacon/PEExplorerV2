#pragma once

#include "StructureView.h"
#include "PEParser.h"

class ImageOptionalHeaderStruct : public IStructureProvider {
public:
	ImageOptionalHeaderStruct(PEParser* parser);

	// IStructureProvider
	CString GetName() const override;
	int GetMemberCount() const override;
	const MemberInfo& GetMemberInfo(int index) const override;

private:
	void BuildMembers();
	void BuildMemberInternal32();
	void BuildMemberInternal64();

private:
	PEParser* _parser;
	std::vector<MemberInfo> _members;
};

