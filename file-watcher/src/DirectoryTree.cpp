#include "../include/DirectoryTree.h"

#include <algorithm>
#include <cassert>

namespace fs
{
	void DirectoryTree::AddDirTreeEventListener(DirectoryTreeEventListener* listener)
	{
		listeners.push_back(listener);
	}
	void DirectoryTree::RemoveDirTreeEventListener(DirectoryTreeEventListener* listener)
	{
		listeners.erase(
			std::remove(listeners.begin(), listeners.end(), listener),
			listeners.end());
	}

	void DirectoryTree::BuildRootTree(const std::filesystem::path& rootDirAbsPath)
	{
		this->rootDirAbsParentPath = rootDirAbsPath.parent_path();

		// "Assets"
		std::filesystem::path rootDirRelPath = rootDirAbsPath.filename();
		rootDir = BuildTree(rootDirRelPath);

		// TEST
		// auto entries = rootDir->GetDirEntries();
		// auto entriesRecursive = rootDir->GetDirEntriesRecursive();

		// TEST
		// auto filesRecursive = rootDir->GetFilesRecursive();
		// auto dirsRecursive = rootDir->GetDirectoriesRecursive();
	}
	
	void DirectoryTree::RemoveSubTree(std::shared_ptr<Directory> dir)
	{

	}

	void DirectoryTree::ClearTree()
	{
		// TODO
		// Don't forget to delete all the entries and notify listeners
		// about each one of them!
		rootDir.reset();
	}

	void DirectoryTree::AddNewFile(const std::filesystem::path& filePath)
	{
		std::filesystem::path parentDirRelPath = filePath.parent_path();

		std::shared_ptr<Directory> parentDir = GetDirectory(parentDirRelPath);
		assert(parentDir && "Can't add a file into a directory that doesn't exist");

		std::shared_ptr<File> newFile = std::make_shared<File>(
			filePath,
			DetectFileAssetType(filePath.extension().generic_string()));

		AddFileToDirectory(parentDir, newFile);

		NotifyFileAdded(newFile);
	}
	void DirectoryTree::AddNewDirectory(const std::filesystem::path& dirPath)
	{
		std::filesystem::path parentDirRelPath = dirPath.parent_path();

		std::shared_ptr<Directory> parentDir = GetDirectory(parentDirRelPath);
		assert(parentDir && "Can't add a directory into a directory that doesn't exist");

		std::shared_ptr<Directory> newDir = BuildTree(dirPath);

		AddDirectoryToDirectory(parentDir, newDir);

		NotifyDirectoryAdded(newDir);
	}
	void DirectoryTree::RemoveFile(const std::filesystem::path& filePath)
	{
		std::filesystem::path parentDirRelPath = filePath.parent_path();

		std::shared_ptr<Directory> parentDir = GetDirectory(parentDirRelPath);
		assert(parentDir && "Can't remove a file from a directory that doesn't exist");

		std::shared_ptr<File> fileToDelete =
			parentDir->GetFile(filePath.filename().generic_string());
		if (!fileToDelete)
			return;

		NotifyFileRemoved(fileToDelete);

		parentDir->DeleteFile(fileToDelete);
	}
	void DirectoryTree::RemoveDirectory(const std::filesystem::path& dirPath)
	{
		std::filesystem::path parentDirRelPath = dirPath.parent_path();

		std::shared_ptr<Directory> parentDir = GetDirectory(parentDirRelPath);
		assert(parentDir && "Can't remove a directory from a directory that doesn't exist");

		std::shared_ptr<Directory> dirToDelete =
			parentDir->GetDirectory(dirPath.filename().generic_string());
		if (!dirToDelete)
			return;

		for (auto& dirEntity : dirToDelete->GetDirEntriesRecursive())
		{
			if (dirEntity->IsDirectory())
			{
				directories.erase(dirEntity->GetPath());
				NotifyDirectoryRemoved(std::static_pointer_cast<Directory>(dirEntity));
			}
			else
			{
				NotifyFileRemoved(std::static_pointer_cast<File>(dirEntity));
			}
		}

		parentDir->DeleteDirectory(dirToDelete);
	}

	void DirectoryTree::ProcessDirectoryTree(DirectoryTreeProcessor* processor)
	{
		if (rootDir)
			processor->ProcessDirectoryTree(rootDir);
	}

	std::shared_ptr<Directory> DirectoryTree::GetDirectory(const std::filesystem::path& dirPath) const
	{
		auto find = directories.find(dirPath);
		if (find == directories.end())
			return std::shared_ptr<Directory>{};

		return find->second;
	}

	std::shared_ptr<Directory> DirectoryTree::GetRootDirectory() const
	{
		return rootDir;
	}

	void DirectoryTree::NotifyDirectoryAdded(std::shared_ptr<Directory> dir)
	{
		std::for_each(
			listeners.begin(), listeners.end(),
			[dir](DirectoryTreeEventListener* listener) {
				listener->OnDirectoryAdded(dir);
		});
	}
	void DirectoryTree::NotifyDirectoryRemoved(std::shared_ptr<Directory> dir)
	{
		std::for_each(
			listeners.begin(), listeners.end(),
			[dir](DirectoryTreeEventListener* listener) {
				listener->OnDirectoryRemoved(dir);
		});
	}

	void DirectoryTree::NotifyFileAdded(std::shared_ptr<File> file)
	{
		std::for_each(
			listeners.begin(), listeners.end(),
			[file](DirectoryTreeEventListener* listener) {
				listener->OnFileAdded(file);
		});
	}
	void DirectoryTree::NotifyFileRemoved(std::shared_ptr<File> file)
	{
		std::for_each(
			listeners.begin(), listeners.end(),
			[file](DirectoryTreeEventListener* listener) {
				listener->OnFileRemoved(file);
		});
	}

	std::shared_ptr<Directory> DirectoryTree::BuildTree(const std::filesystem::path& parentDirPath)
	{
		std::shared_ptr<Directory> parentDir = std::make_shared<Directory>(parentDirPath);
		directories.insert({ parentDirPath, parentDir });
		
		for (const auto& entry : std::filesystem::directory_iterator{ rootDirAbsParentPath / parentDirPath })
		{
			if (entry.is_regular_file())
			{
				std::filesystem::path fileName = entry.path().filename();
				AssetType assetType = DetectFileAssetType(fileName.extension().generic_string());

				std::shared_ptr<File> newFile = std::make_shared<File>(parentDirPath / fileName, assetType);

				AddFileToDirectory(parentDir, newFile);

				NotifyFileAdded(newFile);
			}
			if (entry.is_directory())
			{
				std::filesystem::path dirName = entry.path().filename();
				std::shared_ptr<Directory> newDir = BuildTree(parentDirPath / dirName);

				AddDirectoryToDirectory(parentDir, newDir);

				NotifyDirectoryAdded(newDir);
			}
		}

		return parentDir;
	}
}