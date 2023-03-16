#include "../include/ContentBrowserDrawer.h"

#include <iostream>
#include <Windows.h>

namespace fs
{
	void ContentBrowserDrawer::ProcessDirectoryTree(std::shared_ptr<Directory> root)
	{
		// No flickering issues
		ClearConsole();

		// Flickering issues
		// std::cout << "\x1B[2J\x1B[H";

		DrawDirectory(root, 0);
	}

	// Source:
	// https://stackoverflow.com/questions/5866529/how-do-we-clear-the-console-in-assembly/5866648#5866648
	//
	void ContentBrowserDrawer::ClearConsole()
	{
		char fillChar{ ' ' };

		COORD origin{ 0,0 };
		CONSOLE_SCREEN_BUFFER_INFO screenBufInfo{};

		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

		GetConsoleScreenBufferInfo(console, &screenBufInfo);

		DWORD written{};
		DWORD cells{};

		cells = static_cast<DWORD>(screenBufInfo.dwSize.X * screenBufInfo.dwSize.Y);

		FillConsoleOutputCharacter(console, fillChar, cells, origin, &written);
		FillConsoleOutputAttribute(console, screenBufInfo.wAttributes, cells, origin, &written);

		SetConsoleCursorPosition(console, origin);
	}

	void ContentBrowserDrawer::DrawDirectory(std::shared_ptr<Directory> dir, int nestLevel)
	{
		// Print Directories

		TabulateEntry(nestLevel);
		std::cout << dir->GetDirectoryName() << "\n";

		int nextNestLevel = nestLevel + 1;
		for (const auto& directory : dir->GetDirectories())
		{
			DrawDirectory(directory, nextNestLevel);
		}

		// Print Files

		for (const auto& file : dir->GetFiles())
		{
			TabulateEntry(nextNestLevel);
			std::cout << file->GetFullFileName() << "\n";
		}
	}
	void ContentBrowserDrawer::TabulateEntry(int nestLevel)
	{
		for (int i = 0; i < nestLevel; i++)
		{
			std::cout << "\t";
		}
	}
}