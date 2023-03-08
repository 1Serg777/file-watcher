#pragma once

#include "FileSystemCommon.h"

#include <filesystem>
#include <mutex>
#include <unordered_map>

class IDirectoryTreeProcessor
{
public:
	virtual void ProcessDirectoryTree(std::shared_ptr<Directory> root) = 0;
};

class DirectoryTree
{
public:

	void BuildRootDirectoryTree(const std::filesystem::path& rootDirPath);
	void Clear();

	void ProcessDirectoryTree(IDirectoryTreeProcessor* processor);

	// Return a default constructed shared pointer if the directory doesn't exist
	std::shared_ptr<Directory> GetDirectory(const std::filesystem::path& dirPath);

private:

	void BuildDirectoryTree(std::shared_ptr<Directory> dir);

	std::unordered_map<
		std::filesystem::path,
		std::shared_ptr<Directory>> directories;

	// "[Absolute_path]/Assets/"
	std::shared_ptr<Directory> rootDir;

	std::mutex dir_tree_mutex;
};