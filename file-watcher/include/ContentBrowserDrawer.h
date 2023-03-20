#pragma once

#include "DirectoryTree.h"

namespace fs
{
	class ContentBrowserDrawer : public DirectoryTreeProcessor
	{
	public:

		void ProcessDirectoryTree(std::shared_ptr<Directory> root) override;

	private:

		void ClearConsole();

		void DrawDirectory(std::shared_ptr<Directory> dir, int nestLevel);
		void TabulateEntry(int nestLevel);

		void PrintModifiedSign(std::shared_ptr<File> file);
	};
}