#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs
{
	enum class FileEventType
	{
		ADDED,
		REMOVED,
		MOVED,
		MODIFIED,
		RENAMED
	};

	struct FileEvent
	{
		static FileEvent CreateAddedEvent(const std::filesystem::path& newPath);
		static FileEvent CreateRemovedEvent(const std::filesystem::path& oldPath);
		static FileEvent CreateMovedEvent(
			const std::filesystem::path& oldPath,
			const std::filesystem::path& newPath);
		static FileEvent CreateModifiedEvent(const std::filesystem::path& oldPath);
		static FileEvent CreateRenamedEvent(
			const std::filesystem::path& oldPath,
			const std::filesystem::path& newPath);

		// ADDED messages use only 'newPath'
		//
		// REMOVED messages use only 'oldPath'
		//
		// MOVED messages use both 'oldPath' and 'newPath'
		// 
		// MODIFIED messages use only 'oldPath
		//
		// RENAMED messages use both 'oldPath' and 'newPath'

		std::filesystem::path oldPath;
		std::filesystem::path newPath;

		FileEventType type;
	};

	enum class AssetType
	{
		MODEL,
		SHADER,
		TEXTURE,
		TEXT_DOC,
		UNDEFINED
	};

	enum class DirectoryEntryType
	{
		DIRECTORY,
		FILE,
		UNDEFINED
	};

	enum class DirEntrySortType
	{
		// [...]_L_TO_H means "Lower to Higher"
		// [...]_H_TO_L means "Higher to Lower"

		ALPHABETICAL_L_TO_H,
		ALPHABETICAL_H_TO_L,
		LAST_WRITE_TIME_L_TO_H,
		LAST_WRITE_TIME_H_TO_L,
	};

	class Directory;

	// DirectoryEntry

	class DirectoryEntry
	{
	public:

		DirectoryEntry(const std::filesystem::path& dirEntryPath);

		virtual bool IsFile() const = 0;
		virtual bool IsDirectory() const = 0;
		virtual DirectoryEntryType GetDirectoryEntryType() const = 0;
		virtual std::string GetUniqueName() const = 0;

		void Rename(const std::string& newName);

		const std::filesystem::path& GetPath() const;
		virtual void UpdatePath();

		void SetParentDirectory(std::shared_ptr<Directory> parentDir);
		void ClearParentDirectory();

		bool Exists() const;

		bool LastWriteTimeChanged() const;
		void UpdateLastWriteTime();

		std::filesystem::file_time_type GetLastWriteTime() const;

	protected:

		std::filesystem::path dirEntryPath;

		std::filesystem::file_time_type lastWriteTime{};

		std::weak_ptr<Directory> parentDir;
	};

	class File;
	class Sorter;

	// Directory

	class Directory : public DirectoryEntry
	{
	public:

		using Directories = std::vector<std::shared_ptr<Directory>>;
		using Files = std::vector<std::shared_ptr<File>>;
		using DirectoryEntries = std::vector<std::shared_ptr<DirectoryEntry>>;

		static void AddEntryToDirectory(std::shared_ptr<Directory> dir, std::shared_ptr<DirectoryEntry> entry);
		static void AddDirectoryToDirectory(std::shared_ptr<Directory> where, std::shared_ptr<Directory> what);
		static void AddFileToDirectory(std::shared_ptr<Directory> where, std::shared_ptr<File> file);

		Directory(const std::filesystem::path& dirPath);

		bool IsFile() const override;
		bool IsDirectory() const override;
		DirectoryEntryType GetDirectoryEntryType() const override;
		std::string GetUniqueName() const override;

		void UpdatePath() override;

		void AddDirectoryEntry(std::shared_ptr<DirectoryEntry> entry);
		void AddDirectory(std::shared_ptr<Directory> dir);
		void AddFile(std::shared_ptr<File> file);

		void DeleteDirectoryEntry(std::shared_ptr<DirectoryEntry> entry);
		void DeleteDirectory(const std::string& dirName);
		void DeleteDirectory(std::shared_ptr<Directory> dir);
		void DeleteFile(const std::string& fileName);
		void DeleteFile(std::shared_ptr<File> file);

		// Returns a default constructed 'shared_ptr<Directory>' object
		// if the directory being searched for doesn't exist
		std::shared_ptr<Directory> GetDirectory(const std::string& dirName);
		// Returns a default constructed 'shared_ptr<File>' object
		// if the file being searched for doesn't exist
		std::shared_ptr<File> GetFile(const std::string& fileName);

		bool DirectoryExists(const std::string& dirName);
		bool FileExists(const std::string& fileName);

		std::string GetDirectoryName() const;

		std::vector<std::shared_ptr<Directory>> GetDirectories() const;
		std::vector<std::shared_ptr<Directory>> GetDirectoriesRecursive() const;
		std::vector<std::shared_ptr<File>> GetFiles() const;
		std::vector<std::shared_ptr<File>> GetFilesRecursive() const;

		std::vector<std::shared_ptr<DirectoryEntry>> GetDirEntries() const;
		std::vector<std::shared_ptr<DirectoryEntry>> GetDirEntriesRecursive() const;

		DirEntrySortType GetSortingType() const;
		void SetSortingType(DirEntrySortType sortType);

	private:

		void SortDirectories();
		void SortFiles();

		void InsertDirectorySorted(std::shared_ptr<Directory> dir);
		void InsertFileSorted(std::shared_ptr<File> file);

		Directories directories;
		Files files;

		std::shared_ptr<Sorter> sorter;
		DirEntrySortType sortType{ DirEntrySortType::ALPHABETICAL_L_TO_H };
	};

	// File

	class File : public DirectoryEntry
	{
	public:

		File(
			const std::filesystem::path& filePath,
			AssetType assetType);

		bool IsFile() const override;
		bool IsDirectory() const override;
		DirectoryEntryType GetDirectoryEntryType() const override;
		std::string GetUniqueName() const override;

		void UpdatePath() override;

		std::string GetFullFileName() const;
		std::string GetFileName() const;
		std::string GetFileExtension() const;

		AssetType GetFileAssetType() const;

	private:

		AssetType assetType;
	};

	// Sorters

	class Sorter
	{
	public:

		enum class SortCompFun
		{
			LESS,
			GREATER
		};

		void SetSortingCompFun(SortCompFun comp);
		SortCompFun GetSortingCompFun() const;

		virtual void SortDirectories(
			std::vector<std::shared_ptr<Directory>>& directories) = 0;
		virtual void SortFiles(
			std::vector<std::shared_ptr<File>>& files) = 0;

		virtual void InsertDirectorySorted(
			std::shared_ptr<Directory> directory,
			std::vector<std::shared_ptr<Directory>>& directories) = 0;
		virtual void InsertFileSorted(
			std::shared_ptr<File> file,
			std::vector<std::shared_ptr<File>>& files) = 0;

	private:

		SortCompFun comp{ SortCompFun::LESS };
	};

	class AlphabeticalSorter : public Sorter
	{
	public:

		void SortDirectories(
			std::vector<std::shared_ptr<Directory>>& directories) override;
		void SortFiles(
			std::vector<std::shared_ptr<File>>& files) override;

		void InsertDirectorySorted(
			std::shared_ptr<Directory> directory,
			std::vector<std::shared_ptr<Directory>>& directories) override;
		void InsertFileSorted(
			std::shared_ptr<File> file,
			std::vector<std::shared_ptr<File>>& files) override;
	};

	class LastWriteTimeSorter : public Sorter
	{
	public:

		void SortDirectories(
			std::vector<std::shared_ptr<Directory>>& directories) override;
		void SortFiles(
			std::vector<std::shared_ptr<File>>& files) override;

		void InsertDirectorySorted(
			std::shared_ptr<Directory> directory,
			std::vector<std::shared_ptr<Directory>>& directories) override;
		void InsertFileSorted(
			std::shared_ptr<File> file,
			std::vector<std::shared_ptr<File>>& files) override;
	};

	// Helper methods

	AssetType DetectFileAssetType(std::string_view file_ext);
}