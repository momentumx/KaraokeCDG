#pragma once
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <conio.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <math.h>

#define INCREMENTING_LOOP i =-1; while(++i!= 
#define DECREMENTING_LOOP i = amount; while(--i!= -1

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define clearbuffer cin.clear();cin.ignore(INT_MAX, '\n')

using namespace std;
namespace System
{

// Colors - can be used as Foreground or Background
enum ConsoleColor
{
	Black		= 0,
	DarkBlue	= FOREGROUND_BLUE,
	DarkGreen	= FOREGROUND_GREEN,
	DarkCyan	= FOREGROUND_GREEN | FOREGROUND_BLUE,
	DarkRed		= FOREGROUND_RED,
	DarkMagenta	= FOREGROUND_RED | FOREGROUND_BLUE,
	DarkYellow	= FOREGROUND_RED | FOREGROUND_GREEN,
	Gray		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	DarkGray	= FOREGROUND_INTENSITY,
	Blue		= FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Green		= FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	Cyan		= FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Red			= FOREGROUND_RED | FOREGROUND_INTENSITY,
	Magenta		= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Yellow		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	White		= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
};

class Console
{
	// Data members
	static HANDLE out;
	static WORD defColors;
	static HWND hwnd;

	// Display a Unicode symbol.
	static void Show(wchar_t symbol);

public:
	// Get the current foreground color
	static WORD ForegroundColor();

	// Change the foreground color
	static void ForegroundColor(WORD attr);

	// Get the current background color
	static WORD BackgroundColor();

	// Change the background color
	static void BackgroundColor(WORD attr);

	// Set the colors back to the defaults (usually gray on black)
	static void ResetColor();
	
	// Get the width of the window in cells (characters)
	static int WindowWidth();

	// Get the height of the window in cells (characters)
	static int WindowHeight();

	// Change the size of the window
	static void SetWindowSize(int columns, int rows);

	// Change the size of the buffer
	// Should call this whenever you change the window size
	static void SetBufferSize(int columns, int rows);

	// Get the current column of the cursor
	static int CursorLeft();

	// Get the current row of the cursor
	static int CursorTop();

	// Change the position of the cursor
	static void SetCursorPosition(int left, int top);

	// Clear the screen
	static void Clear();

	// Set the visibility of the cursor
	static void CursorVisible(bool visible);

	// For animation and/or to reduce flickering
	static void Lock(bool lock);

	// Do or don't scroll when printing to the lower-right cell.
	static void EOLWrap(bool on);

	// Prevent cin and GetAsyncKeyState() from seeing each other's input.
	static void FlushKeys();

	// Display a Unicode symbol.
	static void Show(int x, int y, wchar_t symbol);

	// Display a rectangle in the window.
	static void DrawBox(int left, int top, int width, int height, bool dbl);

	// Return a randomly-generated name.
	static char const * RandomName();

	// Show a message inside a rectangular area with word wrap.
	static void WordWrap(int x, int y, int w, char const * const t);
};

class PC
{
	long long freq, start;

public:
	PC(bool go);
	void Start();
	double Finish() const;
};

}
