/*-----------------------------------------------------------------------------------
--	SOURCE FILE:	header.h - Header file of the terminal emulator, defining menu
--							   IDs, global variables, and functions shared between
--							   classes.
--
--	PROGRAM:        Terminal Emulator
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	NOTES:			Header.h is part of a minimal Windows terminal emulator,
--					that transmits characters typed on the keyboard to the serial
--					port and displays all characters received via the serial port.
--
--					This program uses asynchronous I/O to handle the read/write on
--					the serial port.
--
--					This program utilizes the layered (OSI) approach.
-----------------------------------------------------------------------------------*/

#ifndef HEADER_H
#define HEADER_H

#define IDM_Connect		100
#define IDM_Disconnect  101
#define IDM_Exit		102
#define IDM_HELP        103
#define IDM_ConnParams  104
#define IDM_COM1        105
#define IDM_COM2        106
#define IDM_COM3        107
#define IDM_COM4        108
#define IDM_COM5        109
#define IDM_File        110
#define IDM_Ports       111

#define READ_TIMEOUT      500      // milliseconds

// Global variables
extern HANDLE hComm;         // handle for communication port
extern BOOL connected;       // flag to signal if session is connected / disconnected
extern HANDLE readThread;    // handle for read thread
extern HWND hwnd;            // handle for window
extern DWORD readThreadID;  
extern LPCSTR lpszCommName;  // COM port name
extern DCB dcb;
extern HDC hdc;

// Function prototypes
void Connect();
void Disconnect();
BOOL GetCommParameters();
DWORD WINAPI MonitorInputThread(LPVOID hwnd);
void WriteToSerial(WPARAM wParam);
void PrintToScreen(char readBuffer[]);
void SetConnectedUI();
void SetDisconnectedUI();

#endif
