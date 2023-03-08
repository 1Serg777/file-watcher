#include "../include/ContentBrowser.h"

#include <chrono>

using namespace std::chrono;

constexpr int64_t _30_fps_ms{ 33 };
constexpr int64_t _60_fps_ms{ 16 };

ContentBrowser::ContentBrowser(int64_t customFreqMs)
	: timerMs(customFreqMs)
{
	InitializeContentBrowser();
}
ContentBrowser::ContentBrowser(UpdateFreq freq)
{
	SetTimerMs(freq);
	InitializeContentBrowser();
}
ContentBrowser::~ContentBrowser()
{
	StopScanningAssetsRootPath();
}

void ContentBrowser::StartScanningAssetsRootPath()
{
	exec_thread = std::thread{ &ContentBrowser::ScanMainLoop, this };
}
void ContentBrowser::StopScanningAssetsRootPath()
{
	scanning = false;
	WaitForFinish();
}

void ContentBrowser::ProcessDirectoryTree(IDirectoryTreeProcessor* processor)
{
	directoryTree->ProcessDirectoryTree(processor);
}

void ContentBrowser::SetAssetsRootPath(const std::filesystem::path& assetsRootPath)
{
	rootPath = assetsRootPath;
	currentPath = rootPath;
}

void ContentBrowser::InitializeContentBrowser()
{
	contentBrowserDrawer = std::make_unique<ContentBrowserDrawer>();
	directoryTree = std::make_unique<DirectoryTree>();
}

void ContentBrowser::SetTimerMs(UpdateFreq freq)
{
	switch (freq)
	{
	case UpdateFreq::_30_FPS:
		timerMs = _30_fps_ms;
		break;
	case UpdateFreq::_60_FPS:
		timerMs = _60_fps_ms;
		break;
	}
}

void ContentBrowser::ScanMainLoop()
{
	scanning = true;

	// Time Points' types are "std::chrono::time_point<std::chrono::steady_clock>"

	auto startTimePoint = high_resolution_clock::now();
	int64_t currentMs{ timerMs };
	while (scanning)
	{
		if (currentMs >= timerMs)
		{
			updatingTree = true;
			directoryTree->Clear();
			directoryTree->BuildRootDirectoryTree(rootPath);

			ProcessDirectoryTree(contentBrowserDrawer.get());

			startTimePoint = high_resolution_clock::now();
			currentMs = 0;
			updatingTree = false;
		}
		else
		{
			auto endTimePoint = high_resolution_clock::now();
			currentMs = duration_cast<milliseconds>(endTimePoint - startTimePoint).count();
		}
	}
}

void ContentBrowser::WaitForFinish()
{
	if (exec_thread.joinable())
		exec_thread.join();
}
void ContentBrowser::Detach()
{
	if (exec_thread.joinable())
		exec_thread.detach();
}