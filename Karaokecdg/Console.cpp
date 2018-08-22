#include "Console.h"

namespace System
{

HANDLE Console::out = GetStdHandle(STD_OUTPUT_HANDLE);
WORD Console::defColors = Console::BackgroundColor() << 4 | Console::ForegroundColor();
HWND Console::hwnd = GetConsoleWindow();

void Console::Show(wchar_t symbol)
{
	std::wcout << symbol;
}

WORD Console::ForegroundColor()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);
	return info.wAttributes & 0x0F;
}

void Console::ForegroundColor(WORD attr)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);
	SetConsoleTextAttribute(out, info.wAttributes & 0xF0 | attr);
}

WORD Console::BackgroundColor()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);
	return (info.wAttributes & 0xF0) >> 4;
}

void Console::BackgroundColor(WORD attr)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);
	SetConsoleTextAttribute(out, info.wAttributes & 0x0F | attr << 4);
}

void Console::ResetColor()
{
	SetConsoleTextAttribute(out, defColors);
}

int Console::WindowWidth()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);
	return info.srWindow.Right - info.srWindow.Left + 1;
}

int Console::WindowHeight()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);
	return info.srWindow.Bottom - info.srWindow.Top + 1;
}

void Console::SetWindowSize(int columns, int rows)
{
	COORD c = GetLargestConsoleWindowSize(out);
	if (columns < 1 || columns > c.X)
		columns = c.X;
	if (rows < 1 || rows > c.Y)
		rows = c.Y;

	SMALL_RECT dim = { 0, 0, (SHORT)columns - 1, (SHORT)rows - 1 };
	SetConsoleWindowInfo(out, TRUE, &dim);
}

void Console::SetBufferSize(int columns, int rows)
{
	COORD c = { (SHORT)columns, (SHORT)rows };
	SetConsoleScreenBufferSize(out, c);
}

void Console::CursorVisible(bool visible)
{
	CONSOLE_CURSOR_INFO curs;
	GetConsoleCursorInfo(out, &curs);
	if (curs.bVisible == TRUE && visible || curs.bVisible == FALSE && !visible)
		return;
	curs.bVisible = (visible) ? TRUE : FALSE;
	SetConsoleCursorInfo(out, &curs);
}

void Console::Clear()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);

	DWORD len = info.dwSize.X, num;
	COORD c = { 0, 0, };
	for (int i = 0; i < info.dwSize.Y; ++i)
	{
		c.Y = i;
		FillConsoleOutputAttribute(out, info.wAttributes, len, c, &num);
		FillConsoleOutputCharacter(out, ' ', len, c, &num);
	}

	SetCursorPosition(0, 0);
}

int Console::CursorLeft()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);
	return info.dwCursorPosition.X;
}

int Console::CursorTop()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);
	return info.dwCursorPosition.Y;
}

void Console::SetCursorPosition(int left, int top)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(out, &info);

	left = max(left, 0);
	left = min(left, info.dwSize.X - 1);
	top = max(top, 0);
	top = min(top, info.dwSize.Y - 1);

	COORD cp = { (SHORT)left, (SHORT)top, };
	SetConsoleCursorPosition(out, cp);
}

void Console::Lock(bool lock)
{
	LockWindowUpdate(lock ? hwnd : nullptr);
}

void Console::EOLWrap(bool on)
{
	DWORD mode;
	GetConsoleMode(out, &mode);
	if (on)
		mode |= ENABLE_WRAP_AT_EOL_OUTPUT;
	else
		mode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
	SetConsoleMode(out, mode);
}

void Console::FlushKeys()
{
	for (int i = 0; i < 256; ++i) GetAsyncKeyState(i);
	while (_kbhit()) _getch();
}

void Console::Show(int x, int y, wchar_t symbol)
{
	static bool imbued;
	if (!imbued)
	{
		std::wcout.imbue(std::locale(".OCP"));
		imbued = true;
	}
	SetCursorPosition(x, y);
	Show(symbol);
}

void Console::DrawBox(int left, int top, int width, int height, bool dbl)
{
	wchar_t const * SingleLine = L"┌─┐│└┘",
		*DoubleLine = L"╔═╗║╚╝",
		*Set = dbl ? DoubleLine : SingleLine;

	Show(left, top, Set[0]);
	for (int col = 0; col < width - 2; col++)
		Show(Set[1]);
	Show(Set[2]);

	int x = left + width - 1, y = top + 1;
	for (int row = 0; row < height - 2; row++, y++)
	{
		Show(left, y, Set[3]);
		Show(x, y, Set[3]);
	}

	y = top + height - 1;
	Show(left, y, Set[4]);
	for (int col = 0; col < width - 2; col++)
		Show(Set[1]);
	Show(Set[5]);
}

char const * Console::RandomName()
{
	const int Min = 3, Max = 7;
	static char name[Max + 1];

	const char *Cons = "bcdfghjklmnpqrstvwxyz", *Vows = "aeiouy";
	const int NumCons = strlen(Cons), NumVows = strlen(Vows);

	int nameLen = rand() % (Max - Min + 1) + Min;
	bool putAConsHere = rand() % 2 != 0;
	for (int i = 0; i < nameLen; i++, putAConsHere = !putAConsHere)
		name[i] = putAConsHere ? Cons[rand() % NumCons] : Vows[rand() % NumVows];

	name[nameLen] = '\0';
	name[0] = name[0] - 'a' + 'A';
	return name;
}

void Console::WordWrap(int x, int y, int w, char const * const t)
{
	auto count = strlen(t);
	for (decltype(count) i = 0, j; i < count; i = j)
	{
		for (j = i + w; j < count && ' ' != t[j]; --j);
		if (j > count)
			j = count;

		Console::SetCursorPosition(x, y++);
		for (auto k = i; k < j; ++k)
			std::cout << t[k];
		for (; ' ' == t[j]; ++j);
	}
}

PC::PC(bool go)
{
	QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
	if (go)
		Start();
}

void PC::Start()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&start);
}

double PC::Finish() const
{
	long long finish;
	QueryPerformanceCounter((LARGE_INTEGER *)&finish);
	return (finish - start) / double(freq);
}

}
