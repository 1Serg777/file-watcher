#include "../include/ContentBrowser.h"

#include <cassert>
#include <chrono>

#undef DeleteFile
#undef MoveFile
#undef RemoveDirectory

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
		
		absRootPath = assetsRootPath;
		rootDirAbsParentPath = absRootPath.parent_path();
		relCurrentPath = absRootPath.filename();

		directoryTree->BuildRootTree(absRootPath);
		anyChanges = true;

		fileWatcher->StartWatching(assetsRootPath);
	}
	void ContentBrowser::ClearAssetsDirectory()
	{
		if (!absRootPath.empty())
		{
			fileWatcher->StopWatching();
			directoryTree->ClearTree();

			relCurrentPath.clear();
			absRootPath.clear();
			rootDirAbsParentPath.clear();
		}
	}

	void ContentBrowser::OnDirectoryAdded(std::shared_ptr<Directory> dir)
	{
		// TODO?
	}
	void ContentBrowser::OnDirectoryRemoved(std::shared_ptr<Directory> dir)
	{
		// TODO?
	}

	void ContentBrowser::OnFileAdded(std::shared_ptr<File> file)
	{
		// ADD ASSETS?
	}
	void ContentBrowser::OnFileRemoved(std::shared_ptr<File> file)
	{
		// REMOVE ASSETS?
	}

	void ContentBrowser::InitializeContentBrowser()
	{
		contentBrowserDrawer = std::make_unique<ContentBrowserDrawer>();
		directoryTree = std::make_unique<DirectoryTree>();

		fileWatcher = std::make_unique<FileSystemWatcher>();
	}

	void ContentBrowser::ProcessFileEvent(const FileEvent& fileEvent)
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
				// TODO
				// ProcessFileModifiedEvent(fileEvent);
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
	void ContentBrowser::ProcessFileAddedEvent(const FileEvent& fileEvent) 
	{
		std::filesystem::path rootPathName = absRootPath.filename();
		if (fileEvent.newPath.has_extension())
		{
			directoryTree->AddNewFile(rootPathName / fileEvent.newPath);
		}
		else
		{
			directoryTree->AddNewDirectory(rootPathName / fileEvent.newPath);
		}
	}
	void ContentBrowser::ProcessFileRemovedEvent(const FileEvent& fileEvent)
	{
		std::filesystem::path rootPathName = absRootPath.filename();
		if (fileEvent.oldPath.has_extension())
		{
			directoryTree->RemoveFile(rootPathName / fileEvent.oldPath);
		}
		else
		{
			directoryTree->RemoveDirectory(rootPathName / fileEvent.oldPath);
		}
	}
	void ContentBrowser::ProcessFileMovedEvent(const FileEvent& fileEvent)
	{
		std::filesystem::path rootPathName = absRootPath.filename();
		if (fileEvent.oldPath.has_extension() && fileEvent.newPath.has_extension()) // check both, just in case
		{
			directoryTree->MoveFile(rootPathName / fileEvent.oldPath, rootPathName / fileEvent.newPath);
		}
		else
		{
			directoryTree->MoveDirectory(rootPathName / fileEvent.oldPath, rootPathName / fileEvent.newPath);
		}
	}
	void ContentBrowser::ProcessFileModifiedEvent(const FileEvent& fileEvent)
	{
		// TODO
	}
	void ContentBrowser::ProcessFileRenamedEvent(const FileEvent& fileEvent)
	{
		std::filesystem::path rootPathName = absRootPath.filename();
		if (fileEvent.oldPath.has_extension() && fileEvent.newPath.has_extension()) // check both, just in case
		{
			directoryTree->RenameFile(rootPathName / fileEvent.oldPath, rootPathName / fileEvent.newPath);
		}
		else
		{
			directoryTree->RenameDirectory(rootPathName / fileEvent.oldPath, rootPathName / fileEvent.newPath);
		}
	}
}