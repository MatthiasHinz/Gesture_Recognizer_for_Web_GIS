#include <windows.h>


#pragma once
class GUIConnector
{
public:
	GUIConnector(void);
	~GUIConnector(void);

	//synthesizes a keyboard shortcut from an array of n virtual keycodes
	static int sendKeyboardInput(BYTE keycodes[], int n);
	static const BYTE K = 0x4B;
	static const BYTE L = 0x4C;
	static const BYTE E = 0x45;

	//letters
	//http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731
	static int pressLeftMouseButton(void);
	static int releaseLeftMouseButton(void);
	static int clickLeftMouseButton(void);
};

