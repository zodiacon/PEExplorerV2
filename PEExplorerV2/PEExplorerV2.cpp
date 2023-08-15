// PEExplorerV2.cpp : main source file for PEExplorerV2.exe
//

#include "pch.h"

#include "resource.h"

#include "aboutdlg.h"
#include "MainFrm.h"

CAppModule _Module;

int Run(LPTSTR lpstrCmdLine, int nCmdShow) {
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	auto frame = new CMainFrame();

	if (frame->CreateEx() == nullptr) {
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	// Process the commandline:
	// Open the first given parameter as a file
	// If the param starts with double-quotes, make sure to read the entire parameter, including spaces
	auto len = _tcslen(lpstrCmdLine);
	if (len > 0) {
		CString fileToOpen(lpstrCmdLine);
		if (fileToOpen[0] == '"') {
			fileToOpen.Delete(0, 1);
			// Search for the closing quote:
			auto idxStart = fileToOpen.Find(_T('"'));
			fileToOpen.Delete(idxStart, fileToOpen.GetLength() - idxStart);
		}
		frame->OpenDocument(fileToOpen, false);
	}

	frame->ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow) {
	HRESULT hRes = ::CoInitialize(nullptr);
	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(nullptr, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
