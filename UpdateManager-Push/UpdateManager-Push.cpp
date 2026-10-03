#include <iostream>
#include <string>
#include "PushManager.h"

int main(int argc, char* argv[])
{
    std::string artifactSource;
    std::string artifactTarget;
    std::string hostIp;
    std::string hostUser;
    std::string hostPass;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--artifact-source" && i + 1 < argc)
        {
            artifactSource = argv[++i];
        }
        else if (arg == "--artifact-target" && i + 1 < argc)
        {
            artifactTarget = argv[++i];
        }
        else if (arg == "--host-ip" && i + 1 < argc)
        {
            hostIp = argv[++i];
        }
        else if (arg == "--host-user" && i + 1 < argc)
        {
            hostUser = argv[++i];
        }
        else if (arg == "--host-pass" && i + 1 < argc)
        {
            hostPass = argv[++i];
        }
    }

    if (artifactSource.empty() || artifactTarget.empty() || hostIp.empty() || hostUser.empty() || hostPass.empty())
    {
        std::cerr << "Usage: UpdateManager - Push"
            " --artifact-source <path>"
            " --artifact-target <path>"
            " --host-ip <ip>"
            " --host-user <user>"
            " --host-pass <pass>\n";

        return 1;
    }

    PushManager manager(artifactSource, artifactTarget, hostIp, hostUser, hostPass);
    manager.run();

    return 0;
}
