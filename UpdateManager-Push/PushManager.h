#pragma once

#include <iostream>
#include <string>

class PushManager
{
public:
    PushManager(
        const std::string& artifactSource,
        const std::string& artifactTarget,
        const std::string& hostIp,
        const std::string& username,
        const std::string& password);

    void run();

private:
    std::string m_sArtifactSource;
    std::string m_sArtifactTarget;
    std::string m_sHostIp;
    std::string m_sUsername;
    std::string m_sPassword;
};
