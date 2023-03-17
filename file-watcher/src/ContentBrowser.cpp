#include "../include/ContentBrowser.h"

#include <cassert>
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
			FileEvent fileEvent = fileWatcher->RetrieveFileEvent();
			ProcessFileEvent(fileEvent);
			PrintFileEvent(fileEvent);
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

		directoryTree->BuildRootTree(rootPath);
		anyChanges = true;

		fileWatcher->StartWatching(assetsRootPath);
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

	void ContentBrowser::InitializeContentBrowser()
	{
		contentBrowserDrawer = std::make_unique<ContentBrowserDrawer>();
		directoryTree = std::make_unique<DirectoryTree>();

		fileWatcher = std::make_unique<FileSystemWatcher>();
	}

	void ContentBrowser::ProcessFileEvent(const FileEvent& fileEvent) const
	{
		switch (fileEvent.type)
		{
			case FileEventType::ADDED:
			{
				ProcessFileAddedEvent(fileEvent);
			}
			break;
			case FileEventType::REMOVED:
			{
				ProcessFileRemovedEvent(fileEvent);
			}
			break;
			case FileEventType::MOVED:
			{
				ProcessFileMovedEvent(fileEvent);
			}
			break;
			case FileEventType::MODIFIED:
			{
				ProcessFileModifiedEvent(fileEvent);
			}
			break;
			case FileEventType::RENAMED:
			{
				ProcessFileRenamedEvent(fileEvent);
			}
			break;
			default:
			{
				assert(false && "Unexpected File Event type");
			}
			break;
		}
	}
	void ContentBrowser::ProcessFileAddedEvent(const FileEvent& fileEvent)  const
	{
		std::filesystem::path addedDirEntityAbsPath = rootPath / fileEvent.newPath;
		std::filesystem::path parentDirAbsPath = addedDirEntityAbsPath.parent_path();

		std::shared_ptr<Directory> parentDir = directoryTree->GetDirectory(parentDirAbsPath);

		assert(parentDir && "Can't add an entity into a directory that doesn't exist");

		if (std::filesystem::is_directory(addedDirEntityAbsPath))
		{
			if (parentDir->DirectoryExists(addedDirEntityAbsPath.filename().generic_string()))
				return;

			std::shared_ptr<Directory> newSubTree = directoryTree->BuildSubTree(addedDirEntityAbsPath);
			assert(newSubTree && "Couldn't add a subtree to the root tree");
		}
		else if (std::filesystem::is_regular_file(addedDirEntityAbsPath))
		{
			if (parentDir->FileExists(addedDirEntityAbsPath.filename().generic_string()))
				return;

			std::shared_ptr<File> file = std::make_shared<File>(
				addedDirEntityAbsPath,
				DetectFileAssetType(addedDirEntityAbsPath.extension().generic_string()));

			AddFileToDirectory(parentDir, file);
		}
		else
		{
			assert(false && "Unexpected directory entity was added");
		}
	}
	void ContentBrowser::ProcessFileRemovedEvent(const FileEvent& fileEvent) const
	{
		// TODO
	}
	void ContentBrowser::ProcessFileMovedEvent(const FileEvent& fileEvent) const
	{
		// TODO
	}
	void ContentBrowser::ProcessFileModifiedEvent(const FileEvent& fileEvent) const
	{
		// TODO
	}
	void ContentBrowser::ProcessFileRenamedEvent(const FileEvent& fileEvent) const
	{
		// TODO
	}
}