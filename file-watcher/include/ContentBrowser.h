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
	class ContentBrowser
	{
	public:

		ContentBrowser();
		~ContentBrowser();

		void Tick();
		void DrawGUI();

		void SetAssetsDirectory(const std::filesystem::path& assetsRootPath);
		void ClearAssetsDirectory();

	private:

		void InitializeContentBrowser();

		void ProcessFileEvent(const FileEvent& fileEvent) const;
		void ProcessFileAddedEvent(const FileEvent& fileEvent) const;
		void ProcessFileRemovedEvent(const FileEvent& fileEvent) const;
		void ProcessFileMovedEvent(const FileEvent& fileEvent) const;
		void ProcessFileModifiedEvent(const FileEvent& fileEvent) const;
		void ProcessFileRenamedEvent(const FileEvent& fileEvent) const;

		std::filesystem::path currentPath;
		std::filesystem::path rootPath;

		std::unique_ptr<ContentBrowserDrawer> contentBrowserDrawer;
		std::unique_ptr<DirectoryTree> directoryTree;

		std::unique_ptr<FileSystemWatcher> fileWatcher;

		bool anyChanges{ false };
	};
}