#include "PullManager.h"


PullManager::PullManager(const std::string& updateUrl, const std::string& versionCheckUrl, const std::string& outputFile, const std::string& versionFile,
    const std::string& targetVersion, const std::string& username, const std::string& password)
    : updateUrl(updateUrl)
    , versionCheckUrl(versionCheckUrl)
    , outputFile(outputFile)
    , versionFile(versionFile)
    , targetVersion(targetVersion)
    , username(username)
    , password(password)
{
}

bool PullManager::DownloadFile(const std::string& url, const std::string& outputPath)
{
    std::cout << "Downloading from URL: " << url << " to " << outputPath << "\n";
    HINTERNET hInternet = InternetOpen("PullManager", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
    {
        throw std::runtime_error("Failed to open internet connection.");
    }

    HINTERNET hConnect = InternetOpenUrl(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hConnect)
    {
        InternetCloseHandle(hInternet);
        throw std::runtime_error("Failed to connect to URL: " + url);
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile)
    {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        throw std::runtime_error("Failed to open output file: " + outputPath);
    }

    char buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
    {
        outFile.write(buffer, bytesRead);
    }

    outFile.close();
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    if (!fs::exists(outputPath))
    {
        throw std::runtime_error("Downloaded file not found at: " + outputPath);
    }

    std::cout << "Download completed.\n";
    return true;
}

bool PullManager::ExecuteFile()
{
    std::cout << "Executing file: " << outputFile << "\n";
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!fs::exists(outputFile))
    {
        throw std::runtime_error("Executable file not found: " + outputFile);
    }

    if (!CreateProcess(outputFile.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        throw std::runtime_error("Failed to execute file: " + outputFile);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    std::cout << "Execution completed.\n";
    return true;
}

std::string PullManager::ReadLocalVersion()
{
    std::ifstream file(versionFile);
    if (!file)
    {
        throw std::runtime_error("Failed to read local version file: " + versionFile);
    }

    std::string version;
    std::getline(file, version);
    file.close();
    return version;
}

void PullManager::Rollback()
{
    std::cout << "Rolling back update...\n";
}

void PullManager::CheckAndUpdate(bool downloadOnly)
{
    try
    {
        std::cout << "Checking for updates...\n";
        std::string localVersion = ReadLocalVersion();

        if (localVersion >= targetVersion)
        {
            std::cout << "No updates available.\n";
            return;
        }

        std::cout << "Downloading update...\n";
        if (!DownloadFile(updateUrl, outputFile))
        {
            throw std::runtime_error("Failed to download update.");
        }

        if (downloadOnly)
        {
            std::cout << "Update downloaded but not installed.\n";
            return;
        }

        std::cout << "Executing update...\n";
        if (!ExecuteFile())
        {
            throw std::runtime_error("Failed to execute update.");
        }

        std::ofstream versionOut(versionFile);
        if (!versionOut)
        {
            throw std::runtime_error("Failed to update version file: " + versionFile);
        }

        versionOut << targetVersion;
        versionOut.close();
        std::cout << "Update completed successfully.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        Rollback();
    }
}
