#pragma once

#include "ContentBrowserDrawer.h"
#include "DirectoryTree.h"
#include "FileSystemWatcher.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>

namespace fs
{
	class ContentBrowser : public DirectoryTreeEventListener
	{
	public:

		ContentBrowser();
		~ContentBrowser();

		void Tick();
		void DrawGUI();

		void SetAssetsDirectory(const std::filesystem::path& assetsRootPath);
		void ClearAssetsDirectory();

	private:

		void OnDirectoryAdded(std::shared_ptr<Directory> dir) override;
		void OnDirectoryRemoved(std::shared_ptr<Directory> dir) override;

		void OnFileAdded(std::shared_ptr<File> file) override;
		void OnFileRemoved(std::shared_ptr<File> file) override;

		void InitializeContentBrowser();

		void ProcessFileEvent(const FileEvent& fileEvent);
		void ProcessFileAddedEvent(const FileEvent& fileEvent);
		void ProcessFileRemovedEvent(const FileEvent& fileEvent);
		void ProcessFileMovedEvent(const FileEvent& fileEvent);
		void ProcessFileModifiedEvent(const FileEvent& fileEvent);
		void ProcessFileRenamedEvent(const FileEvent& fileEvent);

		std::filesystem::path relCurrentPath;
		std::filesystem::path absRootPath;
		std::filesystem::path rootDirAbsParentPath;

		std::unique_ptr<ContentBrowserDrawer> contentBrowserDrawer;
		std::unique_ptr<DirectoryTree> directoryTree;

		std::unique_ptr<FileSystemWatcher> fileWatcher;

		bool anyChanges{ false };
	};
}