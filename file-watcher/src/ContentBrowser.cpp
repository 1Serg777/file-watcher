#include "../include/ContentBrowser.h"

#include <cassert>
#include <chrono>
#include <iostream>

#undef DeleteFile
#undef MoveFile
#undef RemoveDirectory

using namespace std::chrono;

namespace fs
{
	constexpr std::string_view png_ext{ ".png" };
	constexpr std::string_view jpg_ext{ ".jpg" };
	constexpr std::string_view jpeg_ext{ ".jpeg" };
	constexpr std::string_view hdr_ext{ ".hdr" };

	constexpr std::string_view glb_ext{ ".glb" };
	constexpr std::string_view gltf_ext{ ".gltf" };
	constexpr std::string_view mtl_ext{ ".mtl" };
	constexpr std::string_view obj_ext{ ".obj" };
	constexpr std::string_view stl_ext{ ".stl" };

	constexpr std::string_view fs_ext{ ".fs" };
	constexpr std::string_view frag_ext{ ".frag" };
	constexpr std::string_view gs_ext{ ".gs" };
	constexpr std::string_view geom_ext{ ".geom" };
	constexpr std::string_view vs_ext{ ".vs" };
	constexpr std::string_view vert_ext{ ".vert" };

	constexpr std::string_view shader_ext{ ".shader" };

	constexpr std::string_view txt_doc_ext{ ".txt" };

	// Asset Type enumeration

	std::unordered_map<std::string_view, AssetType> file_exts_to_asset_type_map
	{
		{ png_ext, AssetType::TEXTURE },
		{ jpg_ext, AssetType::TEXTURE },
		{ jpeg_ext, AssetType::TEXTURE },
		{ hdr_ext, AssetType::TEXTURE },

		{ glb_ext, AssetType::MODEL },
		{ gltf_ext, AssetType::MODEL },
		{ mtl_ext, AssetType::MODEL },
		{ obj_ext, AssetType::MODEL },
		{ stl_ext, AssetType::MODEL },

		/*
		{ fs_ext, AssetType::SHADER },
		{ frag_ext, AssetType::SHADER },
		{ gs_ext, AssetType::SHADER },
		{ geom_ext, AssetType::SHADER },
		{ vs_ext, AssetType::SHADER },
		{ vert_ext, AssetType::SHADER },
		*/

		{ shader_ext, AssetType::SHADER },

		{ txt_doc_ext, AssetType::TEXT_DOC }
	};

	// Content Browser

	ContentBrowser::ContentBrowser()
	{
		InitializeContentBrowser();
	}
	ContentBrowser::~ContentBrowser()
	{
		ClearAssetsDirectory();
	}

	void ContentBrowser::Tick()
	{
		// TODO
		// Process all pending events in FileSystemWatcher?

		ProcessKeyboardEvents();

		if (!fileWatcher->HasFileEvents())
		{
			anyChanges = false;
			return;
		}

		while (fileWatcher->HasFileEvents())
		{
			FileEvent fileEvent = fileWatcher->RetrieveFileEvent();
			ProcessFileEvent(fileEvent);
			// PrintFileEvent(fileEvent);
		}

		anyChanges = true;
	}
	void ContentBrowser::DrawGUI()
	{
		if (anyChanges)
		{
			directoryTree->ProcessDirectoryTree(contentBrowserDrawer.get());
		}
	}

	void ContentBrowser::SetAssetsDirectory(const std::filesystem::path& assetsRootPath)
	{
		ClearAssetsDirectory();
		
		absRootPath = assetsRootPath;
		rootDirAbsParentPath = absRootPath.parent_path();
		relCurrentPath = absRootPath.filename();

		directoryTree->BuildRootTree(absRootPath);
		anyChanges = true;

		fileWatcher->StartWatching(assetsRootPath);
	}
	void ContentBrowser::ClearAssetsDirectory()
	{
		if (!absRootPath.empty())
		{
			fileWatcher->StopWatching();
			directoryTree->ClearTree();

			relCurrentPath.clear();
			absRootPath.clear();
			rootDirAbsParentPath.clear();
		}
	}

	void ContentBrowser::OnDirectoryAdded(std::shared_ptr<Directory> dir)
	{
		// TODO?
	}
	void ContentBrowser::OnDirectoryRemoved(std::shared_ptr<Directory> dir)
	{
		// TODO?
	}

	void ContentBrowser::OnFileAdded(std::shared_ptr<File> file)
	{
		// ADD ASSETS?
	}
	void ContentBrowser::OnFileRemoved(std::shared_ptr<File> file)
	{
		// REMOVE ASSETS?
	}

	void ContentBrowser::OnFileModified(std::shared_ptr<File> file)
	{
		// PROCESS MODIFIED FILES?
	}
	void ContentBrowser::OnDirectoryModified(std::shared_ptr<Directory> dir)
	{
		// PROCESS MODIFIED DIRECTORIES?
	}

	void ContentBrowser::InitializeContentBrowser()
	{
		contentBrowserDrawer = std::make_unique<ContentBrowserDrawer>();
		directoryTree = std::make_unique<DirectoryTree>();

		fileWatcher = std::make_unique<FileSystemWatcher>();
	}

	void ContentBrowser::ProcessFileEvent(const FileEvent& fileEvent)
	{
		switch (fileEvent.type)
		{
			case FileEventType::ADDED:
			{
				ProcessFileAddedEvent(fileEvent);
			}
			break;
			case FileEventType::REMOVED:
			{
				ProcessFileRemovedEvent(fileEvent);
			}
			break;
			case FileEventType::MOVED:
			{
				ProcessFileMovedEvent(fileEvent);
			}
			break;
			case FileEventType::MODIFIED:
			{
				ProcessFileModifiedEvent(fileEvent);
			}
			break;
			case FileEventType::RENAMED:
			{
				ProcessFileRenamedEvent(fileEvent);
			}
			break;
			default:
			{
				assert(false && "Unexpected File Event type");
			}
			break;
		}
	}
	void ContentBrowser::ProcessFileAddedEvent(const FileEvent& fileEvent) 
	{
		std::filesystem::path rootPathName = absRootPath.filename();
		if (fileEvent.newPath.has_extension())
		{
			directoryTree->AddNewFile(rootPathName / fileEvent.newPath);
		}
		else
		{
			directoryTree->AddNewDirectory(rootPathName / fileEvent.newPath);
		}
	}
	void ContentBrowser::ProcessFileRemovedEvent(const FileEvent& fileEvent)
	{
		std::filesystem::path rootPathName = absRootPath.filename();
		if (fileEvent.oldPath.has_extension())
		{
			directoryTree->RemoveFile(rootPathName / fileEvent.oldPath);
		}
		else
		{
			directoryTree->RemoveDirectory(rootPathName / fileEvent.oldPath);
		}
	}
	void ContentBrowser::ProcessFileMovedEvent(const FileEvent& fileEvent)
	{
		std::filesystem::path rootPathName = absRootPath.filename();
		if (fileEvent.oldPath.has_extension() && fileEvent.newPath.has_extension()) // check both, just in case
		{
			directoryTree->MoveFile(rootPathName / fileEvent.oldPath, rootPathName / fileEvent.newPath);
		}
		else
		{
			directoryTree->MoveDirectory(rootPathName / fileEvent.oldPath, rootPathName / fileEvent.newPath);
		}
	}
	void ContentBrowser::ProcessFileModifiedEvent(const FileEvent& fileEvent)
	{
		std::filesystem::path rootPathName = absRootPath.filename();
		if (fileEvent.oldPath.has_extension())
		{
			directoryTree->ProcessModifiedFile(rootPathName / fileEvent.oldPath);
		}
		else
		{
			directoryTree->ProcessModifiedDirectory(rootPathName / fileEvent.oldPath);
		}
	}
	void ContentBrowser::ProcessFileRenamedEvent(const FileEvent& fileEvent)
	{
		std::filesystem::path rootPathName = absRootPath.filename();
		if (fileEvent.oldPath.has_extension() && fileEvent.newPath.has_extension()) // check both, just in case
		{
			directoryTree->RenameFile(rootPathName / fileEvent.oldPath, rootPathName / fileEvent.newPath);
		}
		else
		{
			directoryTree->RenameDirectory(rootPathName / fileEvent.oldPath, rootPathName / fileEvent.newPath);
		}
	}

	void ContentBrowser::ProcessKeyboardEvents()
	{
		// TODO
	}

	// Helper methods

	AssetType DetectFileAssetType(std::string_view file_ext)
	{
		auto find = file_exts_to_asset_type_map.find(file_ext);
		if (find != file_exts_to_asset_type_map.end())
		{
			return find->second;
		}
		return AssetType::UNDEFINED;
	}
}