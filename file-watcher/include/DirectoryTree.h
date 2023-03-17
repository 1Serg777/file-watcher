#pragma once

#include "FileSystemCommon.h"

#include <filesystem>
#include <mutex>
#include <unordered_map>

namespace fs
{
	class IDirectoryTreeProcessor
	{
	public:
		// 'Directory' and 'File' classes are not multithreading-aware, so please make sure not to store
		// any directories and files inside derived classes of this interface.
		virtual void ProcessDirectoryTree(std::shared_ptr<Directory> root) = 0;
	};

	class DirectoryTree
	{
	public:

		void BuildRootTree(const std::filesystem::path& rootDirPath);
		std::shared_ptr<Directory> BuildSubTree(const std::filesystem::path& dirPath);

		void ClearTree();

		// This is dangerous because we can't know what this object is going to do
		// with our root directory. It can store it, traverse its subderectoires and/or store them as well.
		// So many things that could potentially break our protection of the data that the mutex provides us with.
		void ProcessDirectoryTree(IDirectoryTreeProcessor* processor);

		// Return a default constructed 'std::shared_ptr<Directory>' instance if the directory doesn't exist
		std::shared_ptr<Directory> GetDirectory(const std::filesystem::path& dirPath);

	private:

		std::shared_ptr<Directory> BuildTree(const std::filesystem::path& dirPath);

		std::unordered_map<
			std::filesystem::path,
			std::shared_ptr<Directory>> directories;

		std::shared_ptr<Directory> rootDir;
	};
}