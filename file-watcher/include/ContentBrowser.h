#pragma once

#include "ContentBrowserDrawer.h"
#include "DirectoryTree.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>

class ContentBrowser
{
public:

	enum class UpdateFreq
	{
		_30_FPS,
		_60_FPS
	};

	ContentBrowser(int64_t customFreqMs);
	ContentBrowser(UpdateFreq freq);
	~ContentBrowser();

	void StartScanningAssetsRootPath();
	void StopScanningAssetsRootPath();

	void ProcessDirectoryTree(IDirectoryTreeProcessor* processor);

	void SetAssetsRootPath(const std::filesystem::path& assetsRootPath);

private:

	void InitializeContentBrowser();

	void SetTimerMs(UpdateFreq freq);

	void ScanMainLoop();

	void WaitForFinish();
	void Detach();

	std::filesystem::path currentPath;
	std::filesystem::path rootPath;

	std::unique_ptr<ContentBrowserDrawer> contentBrowserDrawer;
	std::unique_ptr<DirectoryTree> directoryTree;

	std::thread exec_thread;

	int64_t timerMs{ 0 };

	bool scanning{ false };
	bool updatingTree{ false };
};