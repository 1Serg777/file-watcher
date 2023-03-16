#include "../include/ContentBrowser.h"

#include <chrono>

using namespace std::chrono;

namespace fs
{
	ContentBrowser::ContentBrowser()
	{
		InitializeContentBrowser();
	}
	ContentBrowser::~ContentBrowser()
	{
		ClearAssetsDirectory();
	}

	void ContentBrowser::Tick()
	{
		// TODO
		// Process all pending events in FileSystemWatcher?

		if (!fileWatcher->HasFileEvents())
		{
			anyChanges = false;
			return;
		}

		while (fileWatcher->HasFileEvents())
		{
			FileEvent event = fileWatcher->RetrieveFileEvent();
			PrintFileEvent(event);
		}

		anyChanges = true;
	}
	void ContentBrowser::DrawGUI()
	{
		if (anyChanges)
		{
			directoryTree->ProcessDirectoryTree(contentBrowserDrawer.get());
		}
	}

	void ContentBrowser::SetAssetsDirectory(const std::filesystem::path& assetsRootPath)
	{
		ClearAssetsDirectory();
		
		rootPath = assetsRootPath;
		currentPath = rootPath;

		directoryTree->BuildTree(rootPath);
		anyChanges = true;

		fileWatcher->StartWatching(assetsRootPath);
	}

	void ContentBrowser::InitializeContentBrowser()
	{
		contentBrowserDrawer = std::make_unique<ContentBrowserDrawer>();
		directoryTree = std::make_unique<DirectoryTree>();

		fileWatcher = std::make_unique<FileSystemWatcher>();
	}
	void ContentBrowser::ClearAssetsDirectory()
	{
		if (!rootPath.empty())
		{
			fileWatcher->StopWatching();
			directoryTree->ClearTree();

			currentPath.clear();
			rootPath.clear();
		}
	}
}