#pragma once

#include "DirectoryTree.h"

class ContentBrowserDrawer : public IDirectoryTreeProcessor
{
public:

	void ProcessDirectoryTree(std::shared_ptr<Directory> root) override;

private:

	void ClearConsole();

	void DrawDirectory(std::shared_ptr<Directory> dir, int nestLevel);
	void TabulateEntry(int nestLevel);
};