#include "../include/FileSystemWatcher.h"

#include <tchar.h>

namespace fs
{
    FileSystemWatcher::FileSystemWatcher()
    {
        InitializeFileSystemWatcher();
    }
    FileSystemWatcher::~FileSystemWatcher()
    {
        if (watching)
            StopWatching();
    }

    void FileSystemWatcher::StartWatching(const std::filesystem::path& watchPath)
    {
        if (watching)
            StopWatching();

        osFileWatcher->StartWatching(watchPath);
        watching = true;
    }
    void FileSystemWatcher::StopWatching()
    {
        osFileWatcher->StopWatching();
        watching = false;
    }

    void FileSystemWatcher::AddFileEvent(const FileEvent& fileEvent)
    {
        std::lock_guard mutex_guard{ fileEventsMutex };
        fileEvents.push(fileEvent);
    }
    FileEvent FileSystemWatcher::RetrieveFileEvent()
    {
        std::lock_guard mutex_guard{ fileEventsMutex };
        FileEvent fileEvent = fileEvents.front();
        fileEvents.pop();
        return fileEvent;
    }

    bool FileSystemWatcher::HasFileEvents()
    {
        std::lock_guard mutex_guard{ fileEventsMutex };
        return !fileEvents.empty();
    }
    size_t FileSystemWatcher::FileEventsAvailable()
    {
        std::lock_guard mutex_guard{ fileEventsMutex };
        return fileEvents.size();
    }

    void FileSystemWatcher::InitializeFileSystemWatcher()
    {
#ifdef _WIN32
        osFileWatcher = std::make_unique<WinFileSystemWatcher>(this);
#elif  __linux__
        osFileWatcher = std::make_unique<LinuxFileSystemWatcher>(this);
#endif
    }

    // Utility functions

    void FileAdded(const std::wstring& newPath)
    {
        // std::wstring action = GetActionString(FILE_ACTION_ADDED);
        // _tprintf(TEXT("Action: (%s)\n"), action.c_str());

        _tprintf(TEXT("File was added!\n"));
        _tprintf(TEXT("Path: (%s)\n"), newPath.c_str());
        // std::wcout << "Path: " << newPath << std::endl;
    }
    void FileMoved(const std::wstring& oldPath, const std::wstring& newPath)
    {
        _tprintf(TEXT("File was moved!\n"));
        _tprintf(TEXT("Old path: (%s)\n"), oldPath.c_str());
        _tprintf(TEXT("New path: (%s)\n"), newPath.c_str());

        /*
        std::wcout << "File was moved!\n"
            << "Old path: " << oldPath << "\n"
            << "New path: " << newPath << std::endl;
        */
    }
    void FileModified(const std::wstring& modifiedPath)
    {
        _tprintf(TEXT("File was modified!\n"));
        _tprintf(TEXT("File path: (%s)\n"), modifiedPath.c_str());
    }
    void FilesModified(const std::vector<std::wstring>& modifiedPaths)
    {
        _tprintf(TEXT("Several files were modified!\n"));
        for (const std::wstring& path : modifiedPaths)
        {
            FileModified(path);
        }
    }
    void FileRemoved(const std::wstring& oldPath)
    {
        // std::wstring action = GetActionString(FILE_ACTION_REMOVED);
        // _tprintf(TEXT("Action: (%s)\n"), action.c_str());

        _tprintf(TEXT("File was deleted!\n"));
        _tprintf(TEXT("Old path: (%s)\n"), oldPath.c_str());
        // std::wcout << "Old path: " << oldPath << std::endl;
    }
    void FileRenamed(
        const std::wstring& oldName,
        const std::wstring& newName)
    {
        _tprintf(TEXT("File was renamed!\n"));
        _tprintf(TEXT("Old name: (%s)\n"), oldName.c_str());
        _tprintf(TEXT("New name: (%s)\n"), newName.c_str());

        /*
        std::wcout << "File was renamed!\n"
            << "Old name: " << oldName << "\n"
            << "New name: " << newName << std::endl;
        */
    }

    std::wstring GetActionString(size_t action)
    {
        DWORD dwAction = static_cast<DWORD>(action);
        switch (dwAction)
        {
        case FILE_ACTION_ADDED:
            return std::wstring{ L"[FILE_ACTION_ADDED]" };
            break;
        case FILE_ACTION_REMOVED:
            return std::wstring{ L"[FILE_ACTION_REMOVED]" };
            break;
        case FILE_ACTION_MODIFIED:
            return std::wstring{ L"[FILE_ACTION_MODIFIED]" };
            break;
        case FILE_ACTION_RENAMED_OLD_NAME:
            return std::wstring{ L"[FILE_ACTION_RENAMED_OLD_NAME]" };
            break;
        case FILE_ACTION_RENAMED_NEW_NAME:
            return std::wstring{ L"[FILE_ACTION_RENAMED_NEW_NAME]" };
            break;
        }
        return std::wstring{ L"[UNIDENTIFIED_ACTION]" };
    }
    std::wstring GetFileEventString(FileEventType type)
    {
        switch (type)
        {
        case FileEventType::ADDED:
            return std::wstring{ L"[FILE_ADDED_EVENT]" };
            break;
        case FileEventType::REMOVED:
            return std::wstring{ L"[FILE_REMOVED_EVENT]" };
            break;
        case FileEventType::MOVED:
            return std::wstring{ L"[FILE_MOVED_EVENT]" };
            break;
        case FileEventType::MODIFIED:
            return std::wstring{ L"[FILE_MODIFIED_EVENT]" };
            break;
        case FileEventType::RENAMED:
            return std::wstring{ L"[FILE_RENAMED_EVENT]" };
            break;
        }
        return std::wstring{ L"[UNIDENTIFIED_FILE_EVENT]" };
    }

    void PrintFileEvent(const FileEvent& fileEvent)
    {
        std::wstring eventStr = GetFileEventString(fileEvent.type);

        _tprintf(TEXT("\nFile Event Type: %s\n"), eventStr.c_str());
        _tprintf(TEXT("Old Path: %s\n"), fileEvent.oldPath.c_str());
        _tprintf(TEXT("New Path: %s\n"), fileEvent.newPath.c_str());
    }
}