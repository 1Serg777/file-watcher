#include "../include/DirectoryTree.h"

namespace fs
{
	void DirectoryTree::BuildTree(const std::filesystem::path& rootDirPath)
	{
		std::lock_guard mutex_guard{ dir_tree_mutex };
		rootDir = BuildTreeImpl(rootDirPath);
	}
	void DirectoryTree::ClearTree()
	{
		std::lock_guard mutex_guard{ dir_tree_mutex };

		rootDir.reset();
	}

	void DirectoryTree::ProcessDirectoryTree(IDirectoryTreeProcessor* processor)
	{
		std::lock_guard mutex_guard{ dir_tree_mutex };

		if (rootDir)
			processor->ProcessDirectoryTree(rootDir);
	}

	std::shared_ptr<Directory> DirectoryTree::GetDirectory(const std::filesystem::path& dirPath)
	{
		std::lock_guard mutex_guard{ dir_tree_mutex };

		auto find = directories.find(dirPath);
		if (find == directories.end())
			return std::shared_ptr<Directory>{};

		return find->second;
	}

	std::shared_ptr<Directory> DirectoryTree::BuildTreeImpl(const std::filesystem::path& dirPath)
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
				std::shared_ptr<Directory> dirEntry = BuildTreeImpl(entry.path());
				AddDirectoryToDirectory(dir, dirEntry);
			}
		}

		return dir;
	}
}