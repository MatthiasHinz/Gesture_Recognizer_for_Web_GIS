#include "GUIConnector.h"

GUIConnector::GUIConnector(void)
{
}


GUIConnector::~GUIConnector(void)
{
}


int GUIConnector::sendKeyboardInput(BYTE keycodes[], int n)
{
	INPUT input = {0};
	input.type = INPUT_KEYBOARD;
	input.ki.dwFlags = 0;
	int returnCode = 1;

	for (int i = 0; i < n; i++)  // KEYS DOWN!!!
	{
		input.ki.wVk = keycodes[i];
		int tempReturn = SendInput(1, &input, sizeof(input));
		returnCode = tempReturn != 0 ? returnCode : tempReturn;
	}		
	
	input.ki.dwFlags = KEYEVENTF_KEYUP; //KEYS UP!!!!

	for (int i = n-1; i >= 0; i--)
	{	
		input.ki.wVk = keycodes[i];
		int tempReturn = SendInput(1, &input, sizeof(input));
		returnCode = tempReturn != 0 ? returnCode : tempReturn;
	}

	return returnCode;
}
