#pragma once

#include "FileSystemCommon.h"

#ifdef _WIN32
#include "WinFileWatcher.h"
#elif  __linux__
#include "LinuxFileWatcher.h"
#endif

#include <filesystem>
#include <memory>
#include <mutex>
#include <queue>

namespace fs
{
	class FileSystemWatcher
	{
	public:

		FileSystemWatcher();
		~FileSystemWatcher();

		void StartWatching(const std::filesystem::path& watchPath);
		void StopWatching();

		void AddFileEvent(const FileEvent& fileEvent);
		FileEvent RetrieveFileEvent();

		bool HasFileEvents();
		size_t FileEventsAvailable();

	private:

		void InitializeFileSystemWatcher();

#ifdef _WIN32
		std::unique_ptr<WinFileSystemWatcher> osFileWatcher;
#elif  __linux__
		std::unique_ptr<LinuxFileSystemWatcher> osFileWatcher;
#endif

		bool watching{ false };

		std::queue<FileEvent> fileEvents;
		std::mutex fileEventsMutex;
	};

	// Utility functions

	void FileAdded(const std::wstring& newPath);
	void FileMoved(const std::wstring& oldPath, const std::wstring& newPath);
	void FileModified(const std::wstring& modifiedPath);
	void FilesModified(const std::vector<std::wstring>& modifiedPaths);
	void FileRemoved(const std::wstring& oldPath);
	void FileRenamed(
		const std::wstring& oldName,
		const std::wstring& newName);

	std::wstring GetActionString(size_t action);
	std::wstring GetFileEventString(FileEventType type);

	void PrintFileEvent(const FileEvent& fileEvent);
}