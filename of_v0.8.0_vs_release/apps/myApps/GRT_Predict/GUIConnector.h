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
	static const BYTE L = 0x4C; //l-key

	//letters
	//http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731
};

