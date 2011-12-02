/*
	This file is part of Warzone 2100.
	Copyright (C) 1999-2004  Eidos Interactive
	Copyright (C) 2005-2011  Warzone 2100 Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef __INCLUDED_WZAPP_C_H__
#define __INCLUDED_WZAPP_C_H__

// FIXME: NOTE: Figure out what is needed
// TODO Replace this file during Qt merge:
// git checkout origin/qt-trunk lib/framework/wzapp_c.h
#define WZ_THREAD SDL_Thread
#define WZ_MUTEX SDL_mutex
#define WZ_SEMAPHORE SDL_sem
#define wzMutexLock SDL_LockMutex
#define wzMutexUnlock SDL_UnlockMutex
#define wzSemaphoreCreate SDL_CreateSemaphore
#define wzSemaphoreDestroy SDL_DestroySemaphore
#define wzSemaphoreWait SDL_SemWait
#define wzSemaphorePost SDL_SemPost
#define wzThreadJoin(x) SDL_WaitThread(x, NULL)
#define wzMutexDestroy SDL_DestroyMutex
#define wzMutexCreate SDL_CreateMutex
#define wzYieldCurrentThread() SDL_Delay(10)
#define wzThreadCreate SDL_CreateThread
#define wzThreadStart(x)
#define wzGetTicks() SDL_GetTicks()
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_timer.h>

#if 0
struct _wzThread;
struct _wzMutex;
struct _wzSemaphore;

typedef struct _wzThread WZ_THREAD;
typedef struct _wzMutex WZ_MUTEX;
typedef struct _wzSemaphore WZ_SEMAPHORE;

void wzQuit(void);              ///< Quit game
void wzSetCursor(CURSOR index);
void wzScreenFlip(void);	///< Swap the graphics buffers
void wzShowMouse(bool visible); ///< Show the Mouse?
void wzGrabMouse(void);		///< Trap mouse cursor in application window
void wzReleaseMouse(void);	///< Undo the wzGrabMouse operation
bool wzActiveWindow(void);	///< Whether application currently has the mouse pointer over it
int wzGetTicks(void);		///< Milliseconds since start of game
void wzFatalDialog(const char *text);	///< Throw up a modal warning dialog

// Thread related
WZ_THREAD *wzThreadCreate(int (*threadFunc)(void *), void *data);
int wzThreadJoin(WZ_THREAD *thread);
void wzThreadStart(WZ_THREAD *thread);
bool wzIsThreadDone(WZ_THREAD *thread);
void wzYieldCurrentThread(void);
WZ_MUTEX *wzMutexCreate(void);
void wzMutexDestroy(WZ_MUTEX *mutex);
void wzMutexLock(WZ_MUTEX *mutex);
void wzMutexUnlock(WZ_MUTEX *mutex);
WZ_SEMAPHORE *wzSemaphoreCreate(int startValue);
void wzSemaphoreDestroy(WZ_SEMAPHORE *semaphore);
void wzSemaphoreWait(WZ_SEMAPHORE *semaphore);
void wzSemaphorePost(WZ_SEMAPHORE *semaphore);
int wzSemaphoreAvailable(WZ_SEMAPHORE *semaphore);
#endif

#endif
