#pragma once
#include "stdafx.h"

// Console function
class ConsoleFunc
{
public:
	// Contructor
	ConsoleFunc();

	// Goto position (x, y)
	static void gotoxy(int x, int y);

	// Set the visibility of cursor
	static void ShowConsoleCursor(bool showFlag);
};

