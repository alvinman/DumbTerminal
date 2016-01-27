/*-----------------------------------------------------------------------------------
--	SOURCE FILE:	Session.cpp - Session layer of a terminal emulator, handling the
--								  initializing and termination of user sessions.
--
--	PROGRAM:        Terminal Emulator
--
--	FUNCTIONS:
--					void Connect() 
--					void Disconnect()
--					BOOL GetCommParameters()
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
--					up the Session layer of this model, responsible for handling
--					initializing and terminating user sessions, allowing users to
--					connect and disconnect from the COM port.
-----------------------------------------------------------------------------------*/

#define STRICT

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

COMMCONFIG cc;
HANDLE readThread = 0;
BOOL connected = FALSE;
LPCSTR lpszCommName = "COM1";  //default port set to COM1

/*-----------------------------------------------------------------------------------
--	FUNCTION: Connect
--
--	DATE:			October 3, 2015
--					
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		void Connect() 
--
--	RETURNS:		void
--
--	NOTES:			Creates a new read thread to allow the user to start transmitting
--					and receiving characters from the serial port.  Initializes the
--					proper flags to signal a connected state.
-----------------------------------------------------------------------------------*/
void Connect() {

	//clear the readbuffer first to remove stray characters
	if (hComm) {
		if (!PurgeComm(hComm, PURGE_RXABORT)) {
			OutputDebugString("error purging");
		}
	}

	connected = TRUE;
	//check if readThread has already been created
	if (readThread == 0) { //create new thread 
		readThread = CreateThread(NULL, 0, MonitorInputThread, hwnd, 0, NULL);
		if (readThread != 0) {
			connected = TRUE;
		} else {
			connected = FALSE;
		}
	}

	if (readThread == NULL) {
		OutputDebugString("readThread failed to initialize");
	} else {
		OutputDebugString("readThread launched");
		SetConnectedUI();
	}
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: Disconnect
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		void Disconnect()
--
--	RETURNS:		void
--
--	NOTES:			Sets connected flag to signal disconnected state. 
-----------------------------------------------------------------------------------*/
void Disconnect() { 
	connected = FALSE;
	SetDisconnectedUI();
}

/*-----------------------------------------------------------------------------------
--	FUNCTION: GetCommParameters
--
--	DATE:			October 3, 2015
--
--	REVISIONS:		N/A
--
--	DESIGNER:		Alvin Man
--
--	PROGRAMMER:		Alvin Man
--
--	INTERFACE:		BOOL GetCommParameters()
--
--	RETURNS:		BOOL
--
--	NOTES:			Handles changes made to the Communication Config Dialog. Updates
--					the parameters in the dcb and updates hComm with the new params.
-----------------------------------------------------------------------------------*/
BOOL GetCommParameters() {
	cc.dwSize = sizeof(COMMCONFIG);
	cc.wVersion = 0x100;

	if (!CommConfigDialog(lpszCommName, hwnd, &cc)) {
		return false;
	} else {
		dcb.BaudRate = cc.dcb.BaudRate;
		dcb.ByteSize = cc.dcb.ByteSize;
		dcb.Parity = cc.dcb.Parity;
		dcb.StopBits = cc.dcb.StopBits;
		dcb.fRtsControl = cc.dcb.fRtsControl;
		dcb.fOutxCtsFlow = cc.dcb.fOutxCtsFlow;
	}

	// change sizeof(COMMCONFIG) to cc.dwSize
	SetCommConfig(hComm, &cc, sizeof(COMMCONFIG));
	SetCommState(hComm, &(cc.dcb));

	return true;
}