#include <cstdlib>

#include "../include/ContentBrowser.h"
#include "../include/ContentBrowserDrawer.h"

int main(int argc, char* argv[])
{
	std::filesystem::path assetsRootPath = std::filesystem::path{ argv[0] }.remove_filename();
	assetsRootPath = assetsRootPath / "Assets";

	ContentBrowser contentBrowser{ ContentBrowser::UpdateFreq::_30_FPS };
	// ContentBrowser contentBrowser{ ContentBrowser::UpdateFreq::_60_FPS };

	// ContentBrowser contentBrowser{ 1000 };

	contentBrowser.SetAssetsRootPath(assetsRootPath);
	contentBrowser.StartScanningAssetsRootPath();

	// ContentBrowserDrawer contentBrowserDrawer{};

	while (true)
	{
		// contentBrowser.ProcessDirectoryTree(&contentBrowserDrawer);
	}
	return EXIT_SUCCESS;
}