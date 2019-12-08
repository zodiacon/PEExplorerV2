#pragma once

struct ClipboardHelper final {
	static bool CopyText(HWND hWnd, PCWSTR text);
};

