#include <cstdlib>
#include <iostream>

#include "../include/ContentBrowser.h"

using namespace fs;

void PrintUsage();

int main(int argc, char* argv[])
{
	if (argc != 2)
		PrintUsage();

	std::filesystem::path assetsRootPath = std::filesystem::path{ argv[1] };

	ContentBrowser contentBrowser{};
	contentBrowser.SetAssetsDirectory(assetsRootPath);

	while (true)
	{
		contentBrowser.DrawGUI();
		contentBrowser.Tick();
	}

	return EXIT_SUCCESS;
}

void PrintUsage()
{
	std::cout << "Usage: file-watcher [WATCH_PATH]\n";
	std::cout << "Where [WATCH_PATH] is a path watched for file events\n";
}