#include "GUIConnector.h"
#include <stdio.h>

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

	//print errors
	//printf(" %s; code:%i",GetLastError(), returnCode);
	return returnCode;
}



int GUIConnector::pressLeftMouseButton(void)
{
	//printf(" %i ", GetKeyState(VK_LBUTTON));
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_VIRTUALDESK;
	int returnCode=SendInput(1, &input, sizeof(input));
	printf(" %i ", GetKeyState(VK_LBUTTON));
	printf("(Left Mouse Key down / start panning)\n");
	//print errors
	//printf(" %s; code:%i",GetLastError(), returnCode);
	return returnCode;
}


int GUIConnector::releaseLeftMouseButton(void)
{
	INPUT input2= {0};
	input2.type = INPUT_MOUSE;
	input2.mi.dwFlags = MOUSEEVENTF_LEFTUP | MOUSEEVENTF_VIRTUALDESK;
	int returnCode=SendInput(1, &input2, sizeof(input2));
	//printf(" %i ||| %i %i", GetKeyState(VK_LBUTTON), WM_KEYDOWN, WM_KEYUP);
	printf("(Left Mouse Key up / stop panning)\n");
	return returnCode;
}

int GUIConnector::clickLeftMouseButton(void)
{
	int returnCode = pressLeftMouseButton();
	int returnCode2 = releaseLeftMouseButton();
	return returnCode == 0 || returnCode2 == 0 ? 0 : returnCode2;
}
