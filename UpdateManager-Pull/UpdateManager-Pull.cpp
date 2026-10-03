#include "PullManager.h"

int main(int argc, char* argv[])
{
    try
    {
        bool downloadOnly = false;
        std::string username, password, updateUrl, versionCheckUrl, outputFile, versionFile, targetVersion;

        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg == "--download-only")
            {
                downloadOnly = true;
            }
            else if (arg == "--username" && i + 1 < argc)
            {
                username = argv[++i];
            }
            else if (arg == "--password" && i + 1 < argc)
            {
                password = argv[++i];
            }
            else if (arg == "--update-url" && i + 1 < argc)
            {
                updateUrl = argv[++i];
            }
            else if (arg == "--version-url" && i + 1 < argc)
            {
                versionCheckUrl = argv[++i];
            }
            else if (arg == "--output-file" && i + 1 < argc)
            {
                outputFile = argv[++i];
            }
            else if (arg == "--version-file" && i + 1 < argc)
            {
                versionFile = argv[++i];
            }
            else if (arg == "--target-version" && i + 1 < argc)
            {
                targetVersion = argv[++i];
            }
        }

        if (updateUrl.empty() || versionCheckUrl.empty() || outputFile.empty() || versionFile.empty() || targetVersion.empty())
        {
            std::cerr << "Usage: UpdateManager - Pull"
                " --update-url <url>"
                " --version-url <url>"
                " --output-file <path>"
                " --version-file <path>"
                " --target-version <version>\n";
            return 1;
        }

        PullManager updater(updateUrl, versionCheckUrl, outputFile, versionFile, targetVersion, username, password);
        updater.CheckAndUpdate(downloadOnly);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
