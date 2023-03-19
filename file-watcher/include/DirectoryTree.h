#pragma once

#include "FileSystemCommon.h"

#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace fs
{
	class DirectoryTreeProcessor
	{
	public:
		// 'Directory' and 'File' classes are not multithreading-aware, so please make sure not to store
		// any directories and files inside derived classes of this interface.
		virtual void ProcessDirectoryTree(std::shared_ptr<Directory> root) = 0;
	};

	class DirectoryTreeEventListener
	{
	public:
		virtual void OnDirectoryAdded(std::shared_ptr<Directory> dir) = 0;
		virtual void OnDirectoryRemoved(std::shared_ptr<Directory> dir) = 0;

		virtual void OnFileAdded(std::shared_ptr<File> file) = 0;
		virtual void OnFileRemoved(std::shared_ptr<File> file) = 0;
	};

	class DirectoryTree
	{
	public:

		void AddDirTreeEventListener(DirectoryTreeEventListener* listener);
		void RemoveDirTreeEventListener(DirectoryTreeEventListener* listener);

		void BuildRootTree(const std::filesystem::path& rootDirAbsPath);

		void RemoveSubTree(std::shared_ptr<Directory> dir);

		void ClearTree();

		void AddNewFile(const std::filesystem::path& filePath);
		void AddNewDirectory(const std::filesystem::path& dirPath);

		void RemoveFile(const std::filesystem::path& filePath);
		void RemoveDirectory(const std::filesystem::path& dirPath);

		void MoveFile(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);
		void MoveDirectory(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

		void RenameFile(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);
		void RenameDirectory(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

		// This is dangerous because we can't know what this object is going to do
		// with our root directory. It can store it, traverse its subderectoires and/or store them as well.
		// So many things that could potentially break our protection of the data that the mutex provides us with.
		void ProcessDirectoryTree(DirectoryTreeProcessor* processor);

		// Return a default constructed 'std::shared_ptr<Directory>' instance if the directory doesn't exist
		std::shared_ptr<Directory> GetDirectory(const std::filesystem::path& dirPath) const;

		std::shared_ptr<Directory> GetRootDirectory() const;

	private:

		void NotifyDirectoryAdded(std::shared_ptr<Directory> dir);
		void NotifyDirectoryRemoved(std::shared_ptr<Directory> dir);

		void NotifyFileAdded(std::shared_ptr<File> file);
		void NotifyFileRemoved(std::shared_ptr<File> file);

		std::shared_ptr<Directory> BuildTree(const std::filesystem::path& dirPath);

		// Change 'old path' to 'new path' links
		void ResolveMovedDirectoryMapLinks(
			const std::filesystem::path& whereDirMoved,
			const std::filesystem::path& relPathToMovedDir,
			std::shared_ptr<Directory> relPathDir);

		std::unordered_map<
			std::filesystem::path,
			std::shared_ptr<Directory>> directories;

		std::shared_ptr<Directory> rootDir;
		std::filesystem::path rootDirAbsParentPath;

		// Callbacks

		std::vector<DirectoryTreeEventListener*> listeners;
	};
}