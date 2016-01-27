/*-----------------------------------------------------------------------------------
--	SOURCE FILE:	Physical.cpp - Physical layer of the terminal emulator, providing
--								   system level functions, such as setting and
--								   initializing the port, and reading/writing chars
--								   from the port.
--
--	PROGRAM:        Terminal Emulator
--
--	FUNCTIONS:
--					DWORD WINAPI MonitorInputThread(LPVOID hwnd)
--					void WriteToSerial(WPARAM wParam)
--					BOOL SetupComm()
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	NOTES:			Physical.cpp is part of a minimal Windows terminal emulator,
--					that transmits characters typed on the keyboard to the serial
--					port and displays all characters received via the serial port.
--
--					This program uses asynchronous I/O to handle the read/write on
--					the serial port.
--
--					This program utilizes the layered (OSI) approach. This file makes
--					up the Physical layer of this model, responsible for handling
--					system level functionality including setting up the comm port,
--					and receiving / transmiting characters from the serial port.
-----------------------------------------------------------------------------------*/

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

// function prototype
BOOL SetupComm();

// declared variables
HANDLE hComm;
OVERLAPPED o;
DCB dcb;
HDC hdc;

/*-----------------------------------------------------------------------------------
--	FUNCTION: MonitorInputThread
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		DWORD WINAPI MonitorInputThread(LPVOID hwnd)
--
--	RETURNS:		DWORD
--
--	NOTES:			This is the main read thread code that handles receiving
--					characters from the serial port asynchronously.  
-----------------------------------------------------------------------------------*/
DWORD WINAPI MonitorInputThread(LPVOID hwnd) {

	DWORD readBytes;
	DWORD dwRes;
	DWORD readThreadExitCode;
	char readBuffer[80] = "\0\0\0\0\0\0\0\0\0\0";
	BOOL waitingOnRead = FALSE;

	if (!SetupComm()) {
		OutputDebugString("Error occurred while setting up communications");
		return 0;
	}

	// create manual reset event for asynchronous I/O
	o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (o.hEvent == NULL) {
		OutputDebugString("Error creating reset event");
		return 0;
	}

	// read loop
	while (1) {
		//check if the session is still connected
		if (!connected) {
			if (GetExitCodeThread(readThread, &readThreadExitCode) != 0) {

				readThread = 0;
				if (!CloseHandle(hComm)) {
					OutputDebugString("Error closing handle");
				} else {
					OutputDebugString("thread closed");
				}
				
				ExitThread(readThreadExitCode);
			}
		}

		if (!waitingOnRead) {
			//attempt to read the character from the serial port
			if (!ReadFile(hComm, readBuffer, sizeof(readBuffer), &readBytes, &o)) {
				if (GetLastError() != ERROR_IO_PENDING) {
					MessageBox(NULL, "Error reading from serial port", "", MB_OK);
					readThread = 0;
					break;
				} else {
					waitingOnRead = TRUE;
				}
			}
		}

		if (waitingOnRead) {
			//wait for overlapped I/O to complete
			dwRes = WaitForSingleObject(o.hEvent, READ_TIMEOUT);
			switch (dwRes) {
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(hComm, &o, &readBytes, FALSE)) {
					MessageBox(NULL, "Error reading file", "", MB_OK);
					break;
				}
				waitingOnRead = FALSE;
				break;
			case WAIT_TIMEOUT:
				break;
			default:
				break;
			}
		}

		// If we have read characters, print them to the screen
		if (readBytes) {
			PrintToScreen(readBuffer);
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: WriteToSerial
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		void WriteToSerial(WPARAM wParam)
--
--	RETURNS:		void
--
--	NOTES:			Transmits the characters typed on the keyboard to the serial port
--					asynchronously.
-----------------------------------------------------------------------------------*/
void WriteToSerial(WPARAM wParam) {

	DWORD dwWritten;
	DWORD dwRes;
	BOOL fRes;

	// create manual reset event for asynchronous I/O
	o.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (o.hEvent == NULL) {
		MessageBox(NULL, "Error creating reset event", "", MB_OK);
	}

	char str[10] = "";
	sprintf_s(str, "%c", (char)wParam); // Convert char to string

	if (connected) {  //only write chars if connected state is true
		if (!WriteFile(hComm, str, 1, &dwWritten, &o)) {
			if (GetLastError() != ERROR_IO_PENDING) {
				// writing to serial port failed
				MessageBox(NULL, "Error writing file", "", MB_OK);
				fRes = FALSE;
			} else {
				// write operation is pending
				dwRes = WaitForSingleObject(o.hEvent, INFINITE);
				switch (dwRes) {
					// OVERLAPPED Structure's event signaled
					case WAIT_OBJECT_0:
						if (!GetOverlappedResult(hComm, &o, &dwWritten, FALSE)) {
							fRes = FALSE;
						} else {
							//write completed successfully
							fRes = TRUE;
						}
						break;
					default:
						//error occurred in WaitForSingleObject
						fRes = FALSE;
						break;
				}
			}
		} else {
			//writefile completed successfully
			fRes = TRUE;
		}
	}
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: SetupComm
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		BOOL SetupComm()
--
--	RETURNS:		BOOL
--
--	NOTES:			Handles the initializing of the communication handle and the port.
--					Opens with the asynchronous I/O flag.
-----------------------------------------------------------------------------------*/
BOOL SetupComm() {

	if ((hComm = CreateFile(lpszCommName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL)) == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, "Error opening COM port:", "", MB_OK);
		connected = FALSE;
		return false;
	}

	//clear the read buffer so users do not retrieve chars when they press connect
	if (hComm) {
		if (!PurgeComm(hComm, PURGE_RXABORT)) {
			OutputDebugString("error purging");
		}
	}

	if (!GetCommState(hComm, &dcb)) {
		//error getting DC settings
		MessageBox(NULL, "Error initializing DCB", "", MB_OK);
		return false;
	}

	if (!SetCommState(hComm, &dcb)) {
		//error setting commstate
		MessageBox(NULL, "Error setting DCB", "", MB_OK);
		return false;
	}

	return true;
}