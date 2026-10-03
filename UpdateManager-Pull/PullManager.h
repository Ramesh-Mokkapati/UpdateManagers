#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <windows.h>
#include <wininet.h>
#include <filesystem>

#pragma comment(lib, "wininet.lib")

namespace fs = std::filesystem;

class PullManager
{
public:
    PullManager(const std::string& updateUrl, const std::string& versionCheckUrl, const std::string& outputFile, const std::string& versionFile,
        const std::string& targetVersion, const std::string& username = "", const std::string& password = "");

    bool DownloadFile(const std::string& url, const std::string& outputPath);

    bool ExecuteFile();

    std::string ReadLocalVersion();

    void Rollback();

    void CheckAndUpdate(bool downloadOnly);

private:
    std::string     updateUrl;
    std::string     versionCheckUrl;
    std::string     outputFile;
    std::string     versionFile;
    std::string     username;
    std::string     password;
    std::string     targetVersion;
};
