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
// Dedicated server stand-in for win_wndproc.c. The real window procedure
// references client state (cls, in_mouse) that is not linked here.
#include "../game/q_shared.h"
#include "../qcommon/qcommon.h"
#include "../win32/win_local.h"

WinVars_t g_wv;

LRESULT WINAPI MainWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
