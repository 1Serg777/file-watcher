#include "../include/DirectoryTree.h"

namespace fs
{
	void DirectoryTree::BuildRootTree(const std::filesystem::path& rootDirPath)
	{
		rootDir = BuildTree(rootDirPath);
	}
	std::shared_ptr<Directory> DirectoryTree::BuildSubTree(const std::filesystem::path& dirPath)
	{
		std::filesystem::path parentDirPath = dirPath.parent_path();
		std::shared_ptr<Directory> parentDir = GetDirectory(parentDirPath);
		if (!parentDir)
			return std::shared_ptr<Directory>{};

		std::shared_ptr<Directory> subTree = BuildTree(dirPath);
		AddDirectoryToDirectory(parentDir, subTree);
		return subTree;
	}
	
	void DirectoryTree::ClearTree()
	{
		rootDir.reset();
	}

	void DirectoryTree::ProcessDirectoryTree(IDirectoryTreeProcessor* processor)
	{
		if (rootDir)
			processor->ProcessDirectoryTree(rootDir);
	}

	std::shared_ptr<Directory> DirectoryTree::GetDirectory(const std::filesystem::path& dirPath)
	{
		auto find = directories.find(dirPath);
		if (find == directories.end())
			return std::shared_ptr<Directory>{};

		return find->second;
	}

	std::shared_ptr<Directory> DirectoryTree::BuildTree(const std::filesystem::path& dirPath)
	{
		std::shared_ptr<Directory> dir = std::make_shared<Directory>(dirPath);
		directories.insert({ dirPath, dir });

		for (const auto& entry : std::filesystem::directory_iterator{ dirPath })
		{
			if (entry.is_regular_file())
			{
				AssetType assetType = DetectFileAssetType(entry.path().extension().generic_string().c_str());
				std::shared_ptr<File> fileEntry = std::make_shared<File>(entry.path(), assetType);
				AddFileToDirectory(dir, fileEntry);
			}
			if (entry.is_directory())
			{
				std::shared_ptr<Directory> dirEntry = BuildTree(entry.path());
				AddDirectoryToDirectory(dir, dirEntry);
			}
		}

		return dir;
	}
}