/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================

Modified 2026-11-10 in the robFraser1111/quake-3 tree.
This is not an official id Software product. See README.md.
*/
// win_local.h: Win32-specific Quake3 header file

#if defined (_MSC_VER) && (_MSC_VER >= 1200)
#pragma warning(disable : 4201)
#pragma warning( push )
#endif
#include <windows.h>
#if defined (_MSC_VER) && (_MSC_VER >= 1200)
#pragma warning( pop )
#endif

#define	DIRECTSOUND_VERSION	0x0800
#define	DIRECTINPUT_VERSION	0x0800

#include <dinput.h>
#include <dsound.h>
#include <winsock.h>

/* wsipx.h left the Windows SDK. Keep the address layout so the net
   code compiles; IPX sockets fail at runtime on current Windows. */
#if defined(__has_include)
#  if __has_include(<wsipx.h>)
#    include <wsipx.h>
#    define Q3_HAVE_WSIPX_H 1
#  endif
#endif
#ifndef Q3_HAVE_WSIPX_H
#ifndef AF_IPX
#define AF_IPX 6
#endif
#ifndef NSPROTO_IPX
#define NSPROTO_IPX 1000
#endif
typedef struct sockaddr_ipx {
	short sa_family;
	char sa_netnum[4];
	char sa_nodenum[6];
	unsigned short sa_socket;
} SOCKADDR_IPX, *PSOCKADDR_IPX, FAR *LPSOCKADDR_IPX;
#endif

void	IN_MouseEvent (int mstate);

void Sys_QueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr );

void	Sys_CreateConsole( void );
void	Sys_DestroyConsole( void );

char	*Sys_ConsoleInput (void);

qboolean	Sys_GetPacket ( netadr_t *net_from, msg_t *net_message );

// Input subsystem

void	IN_Init (void);
void	IN_Shutdown (void);
void	IN_JoystickCommands (void);

void	IN_Move (usercmd_t *cmd);
// add additional non keyboard / non mouse movement on top of the keyboard move cmd

void	IN_DeactivateWin32Mouse( void);

void	IN_Activate (qboolean active);
void	IN_Frame (void);

// window procedure
LRESULT WINAPI MainWndProc (
    HWND    hWnd,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam);

void Conbuf_AppendText( const char *msg );

void SNDDMA_Activate( void );
int  SNDDMA_InitDS ();

typedef struct
{
	
	HINSTANCE		reflib_library;		// Handle to refresh DLL 
	qboolean		reflib_active;

	HWND			hWnd;
	HINSTANCE		hInstance;
	qboolean		activeApp;
	qboolean		isMinimized;
	OSVERSIONINFO	osversion;

	// when we get a windows message, we store the time off so keyboard processing
	// can know the exact time of an event
	unsigned		sysMsgTime;
} WinVars_t;

extern WinVars_t	g_wv;
