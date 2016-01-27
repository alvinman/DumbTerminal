/*-----------------------------------------------------------------------------------
--	SOURCE FILE:	Application.cpp - Application layer of a terminal emulator,
--                                    providing the interface for users to use
--                                    the program.  
--
--	PROGRAM:        Terminal Emulator
--
--	FUNCTIONS:      
--					int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
-- 						  LPSTR lspszCmdParam, int nCmdShow)
--					LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
--                        WPARAM wParam, LPARAM lParam)
--					void PrintToScreen(char readBuffer[])
--					void SetConnectedUI()
--					void SetDisconnectedUI()
--
--	DATE:			October 3, 2015
--					
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	NOTES:			Application.cpp is part of a minimal Windows terminal emulator,
--					that transmits characters typed on the keyboard to the serial
--					port and displays all characters received via the serial port.
--				
--					This program uses asynchronous I/O to handle the read/write on
--					the serial port.  
--					
--					This program utilizes the layered (OSI) approach. This file makes
--					up the Application layer of this model, responsible for handling
--					all user-facing interfaces and functionality.
-----------------------------------------------------------------------------------*/

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

#pragma warning (disable: 4096)

// function prototype
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// declared variables
static TCHAR Name[] = TEXT("DumbTerminal");
static TCHAR HelpMessage[] = TEXT("This program allows you to make a connection ")
TEXT("between serial ports to transmit characters.\nUse the Communication ")
TEXT("Parameters to set the correct COM settings.\nUse the Port Menu ")
TEXT("to choose a COM Port.\nUse the File menu to Connect and Disconnect ")
TEXT("from the COM ports.");
HWND hwnd;     
WNDCLASSEX Wcl;			
COLORREF backgroundColor = RGB(51, 51, 51);
COLORREF textColor = RGB(179, 255, 0);
HMENU programMenu;
int X = 0, Y = 0; //initial starting coordinates for character printing

/*-----------------------------------------------------------------------------------
--	FUNCTION: WinMain
--
--	DATE:			October 3, 2015
--					
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
-- 						LPSTR lspszCmdParam, int nCmdShow)
--
--	RETURNS:		int
--
--	NOTES:			This is the initial entry point for the program.  It is
--					responsible for the message retrieval and dispatch loop that
--					that provides top-level control for the remainder of execution.
-----------------------------------------------------------------------------------*/
int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
 						  LPSTR lspszCmdParam, int nCmdShow)
{
	MSG Msg;

	// Define a Window class
	Wcl.cbSize = sizeof (WNDCLASSEX);
	Wcl.style = 0; // default style
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); // large icon 
	Wcl.hIconSm = NULL; // use small version of large icon
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  // cursor style

	Wcl.lpfnWndProc = WndProc; // window function
	Wcl.hInstance = hInst; // handle to this instance
	Wcl.hbrBackground = CreateSolidBrush(backgroundColor);
	Wcl.lpszClassName = "Firstclass"; // window class name
	
	Wcl.lpszMenuName = TEXT("MYMENU"); // no class menu 
	Wcl.cbClsExtra = 0;      // no extra memory needed
	Wcl.cbWndExtra = 0; 

	// Register the class
	if (!RegisterClassEx (&Wcl))
		return 0;

	// Create main window
	hwnd = CreateWindow (
		"Firstclass", // name of window class
		Name, // title 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, // window style - non-resizable
		CW_USEDEFAULT,	// X coord
		CW_USEDEFAULT, // Y coord
   		600, // width
		400, // height
		NULL, // no parent window
		NULL, // no menu
		hInst, // instance handle
		NULL // no additional arguments
	);
		
	// Display the window
	ShowWindow (hwnd, nCmdShow);
	UpdateWindow (hwnd);

	// Create the message loop
	while (GetMessage (&Msg, NULL, 0, 0))
	{
   		TranslateMessage (&Msg); // translate keyboard messages
		DispatchMessage (&Msg); // dispatch message and return control to windows
	}

	return Msg.wParam;
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: WndProc
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
--						WPARAM wParam, LPARAM lParam)
--
--	RETURNS:		LRESULT
--
--	NOTES:			Handles all messages sent to the window.
-----------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
                          WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintstruct;

	switch (Message)
	{
		case WM_COMMAND: 
			switch (LOWORD(wParam))
			{
				case IDM_Connect:
					Connect();
					break;
				case IDM_Disconnect:
					Disconnect();
					break;  
				case IDM_ConnParams:
					GetCommParameters();
					break;
				case IDM_COM1:
					lpszCommName = "COM1";
					MessageBox(hwnd, "Port set to COM1", "", MB_OK);
					break;
				case IDM_COM2:
					lpszCommName = "COM2";
					MessageBox(hwnd, "Port set to COM2", "", MB_OK);
					break;
				case IDM_COM3:
					lpszCommName = "COM3";
					MessageBox(hwnd, "Port set to COM3", "", MB_OK);
					break;
				case IDM_COM4:
					lpszCommName = "COM4";
					MessageBox(hwnd, "Port set to COM4", "", MB_OK);
					break;
				case IDM_COM5:
					lpszCommName = "COM5";
					MessageBox(hwnd, "Port set to COM5", "", MB_OK);
					break;
				case IDM_HELP:
					MessageBox(hwnd, HelpMessage, "Help", MB_OK);
					break;
				case IDM_Exit:
					PostQuitMessage(0);
					break;
				}
			break;
		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_ESCAPE:
				if (connected != FALSE) {
					connected = FALSE;
					MessageBox(hwnd, "Disconnected", "", MB_OK);
				}
				break;
			}
			break;
		case WM_CHAR:	// Process keystroke
			WriteToSerial(wParam);
			break;
		case WM_PAINT:		// Process a repaint message
			hdc = BeginPaint(hwnd, &paintstruct); // Acquire DC
			EndPaint(hwnd, &paintstruct); // Release DC
			break;
		case WM_DESTROY:		// message to terminate the program
			PostQuitMessage (0);
		break;
		default: // Let Win32 process all other messages
			return DefWindowProc (hwnd, Message, wParam, lParam);
	}
	return 0;	
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: PrintToScreen
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		void PrintToScreen(char readBuffer[])
--
--	RETURNS:		void
--
--	NOTES:			Handles the printing of characters received via the serial port
--					to the screen.  
-----------------------------------------------------------------------------------*/
void PrintToScreen(char readBuffer[]) {

	SIZE lastCharWidth;
	hdc = GetDC(hwnd);
	RECT windowDimension;

	//determine width of the window
	GetWindowRect(hwnd, &windowDimension);
	int windowWidth = windowDimension.right - windowDimension.left;

	//get the width of the last character read
	GetTextExtentPoint32(hdc, readBuffer, 1, &lastCharWidth);

	//print the characters
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, textColor);
	TextOut(hdc, X, Y, readBuffer, 10);

	//increment the screen paint coordinates by the amount of the last char width
	X += lastCharWidth.cx; 

	//if the printed characters exceed window width, jump to the next line
	if (X > windowWidth - 20) {
		X = 0;
		Y += 15;
	}

	ReleaseDC((HWND)hwnd, hdc); // Release device context
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: SetConnectedUI
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		void SetConnectedUI()
--
--	RETURNS:		void
--
--	NOTES:			Sets certain UI elements to disabled during the connected state,
--					to ensure communication settings are not changed until the user
--					is disconnected.
-----------------------------------------------------------------------------------*/
void SetConnectedUI() {
	programMenu = GetMenu(hwnd);
	EnableMenuItem(programMenu, IDM_COM1, MF_GRAYED);
	EnableMenuItem(programMenu, IDM_COM2, MF_GRAYED);
	EnableMenuItem(programMenu, IDM_COM3, MF_GRAYED);
	EnableMenuItem(programMenu, IDM_COM4, MF_GRAYED);
	EnableMenuItem(programMenu, IDM_COM5, MF_GRAYED);
	EnableMenuItem(programMenu, IDM_Connect, MF_GRAYED);
	EnableMenuItem(programMenu, IDM_Disconnect, MF_ENABLED);
	DrawMenuBar(hwnd);
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: SetDisconnectedUI
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		void SetDisconnectedUI()
--
--	RETURNS:		void
--
--	NOTES:			Sets certain UI elements to enabled after the session is
--					disconnected, returning the ability to make changes to 
--					communication settings.
-----------------------------------------------------------------------------------*/
void SetDisconnectedUI() {
	programMenu = GetMenu(hwnd);
	EnableMenuItem(programMenu, IDM_COM1, MF_ENABLED);
	EnableMenuItem(programMenu, IDM_COM2, MF_ENABLED);
	EnableMenuItem(programMenu, IDM_COM3, MF_ENABLED);
	EnableMenuItem(programMenu, IDM_COM4, MF_ENABLED);
	EnableMenuItem(programMenu, IDM_COM5, MF_ENABLED);
	EnableMenuItem(programMenu, IDM_Connect, MF_ENABLED);
	EnableMenuItem(programMenu, IDM_Disconnect, MF_GRAYED);
	DrawMenuBar(hwnd);
}