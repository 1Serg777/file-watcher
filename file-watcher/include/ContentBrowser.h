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
	enum class AssetType
	{
		MODEL,
		SHADER,
		TEXTURE,
		TEXT_DOC,
		UNDEFINED
	};

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

		void OnFileAdded(std::shared_ptr<File> file) override;
		void OnDirectoryAdded(std::shared_ptr<Directory> dir) override;

		void OnFileRemoved(std::shared_ptr<File> file) override;
		void OnDirectoryRemoved(std::shared_ptr<Directory> dir) override;

		void OnFilePathChanged(std::shared_ptr<File> file, const std::filesystem::path& oldPath) override;
		void OnDirectoryPathChanged(std::shared_ptr<Directory> dir, const std::filesystem::path& oldPath) override;

		void OnFileModified(std::shared_ptr<File> file) override;
		void OnDirectoryModified(std::shared_ptr<Directory> dir) override;

		void InitializeContentBrowser();

		void ProcessFileEvent(const FileEvent& fileEvent);
		void ProcessFileAddedEvent(const FileEvent& fileEvent);
		void ProcessFileRemovedEvent(const FileEvent& fileEvent);
		void ProcessFileMovedEvent(const FileEvent& fileEvent);
		void ProcessFileModifiedEvent(const FileEvent& fileEvent);
		void ProcessFileRenamedEvent(const FileEvent& fileEvent);

		void ProcessKeyboardEvents();

		std::filesystem::path relCurrentPath;
		std::filesystem::path absRootPath;
		std::filesystem::path rootDirAbsParentPath;

		std::unique_ptr<ContentBrowserDrawer> contentBrowserDrawer;
		std::unique_ptr<DirectoryTree> directoryTree;

		std::unique_ptr<FileSystemWatcher> fileWatcher;

		bool anyChanges{ false };
	};

	// Helper methods

	AssetType DetectFileAssetType(std::string_view file_ext);
}