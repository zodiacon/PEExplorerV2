// PEDump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <windows.h>
#include "..\PEParser\PEParser.h"

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		printf("Usage: PEDump [options] <filename>\n");
		return 0;
	}

	PEParser parser(argv[1]);
	if (!parser.IsValid()) {
		printf("Error opening file: %d\n", ::GetLastError());
		return 1;
	}

	printf("PE file: %s\n", argv[1]);

	auto resources = parser.EnumResources();
	for (auto& type : resources) {
		printf("%ws\n", (PCWSTR)type.Name);
		for (auto& item : type.Items)
			printf("\t%ws\n", (PCWSTR)item);
	}
}
