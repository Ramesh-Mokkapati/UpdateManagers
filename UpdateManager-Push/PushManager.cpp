#undef byte  // Removes Windows `byte` typedef

#include "PushManager.h"
#include "SFTPUploader.h"

PushManager::PushManager(const std::string& artifactSource, const std::string& artifactTarget,
    const std::string& hostIp, const std::string& username, const std::string& password)
    : m_sArtifactSource(artifactSource)
    , m_sArtifactTarget(artifactTarget)
    , m_sHostIp(hostIp)
    , m_sUsername(username)
    , m_sPassword(password)
{
}

void PushManager::run()
{
    SFTPUploader ssh(m_sHostIp, m_sUsername, m_sPassword);

    if (ssh.UploadFile(m_sArtifactSource, m_sArtifactTarget))
    {
        std::cout << "Uploaded " << m_sArtifactSource << " to " << m_sHostIp << ":" << m_sArtifactTarget << "\n";
    }
    else
    {
        std::cerr << "Failed to upload " << m_sArtifactSource << " to " << m_sHostIp << "\n";
    }
}
