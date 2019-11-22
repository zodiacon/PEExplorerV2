#include "pch.h"
#include "SortHelper.h"

bool SortHelper::SortStrings(const CString& s1, const CString& s2, bool ascending) {
	return ascending ? s2.CompareNoCase(s1) > 0 : s2.CompareNoCase(s1) < 0;
}

bool SortHelper::SortStrings(const std::string& s1, const std::string& s2, bool ascending) {
	return ascending ? s2.compare(s1) > 0 : s2.compare(s1) < 0;
}
