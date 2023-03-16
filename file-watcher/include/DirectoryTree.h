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
		virtual void ProcessDirectoryTree(std::shared_ptr<Directory> root) = 0;
	};

	class DirectoryTree
	{
	public:

		void BuildTree(const std::filesystem::path& rootDirPath);
		void ClearTree();

		void ProcessDirectoryTree(IDirectoryTreeProcessor* processor);

		// Return a default constructed 'std::shared_ptr<Directory>' instance if the directory doesn't exist
		std::shared_ptr<Directory> GetDirectory(const std::filesystem::path& dirPath);

	private:

		std::shared_ptr<Directory> BuildTreeImpl(const std::filesystem::path& dirPath);

		std::unordered_map<
			std::filesystem::path,
			std::shared_ptr<Directory>> directories;

		std::shared_ptr<Directory> rootDir;

		std::mutex dir_tree_mutex;
	};
}