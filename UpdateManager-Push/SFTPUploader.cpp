#include "SFTPUploader.h"

#include <libssh2.h>
#include <libssh2_sftp.h>
#include <iostream>
#include <fstream>
#include <limits>

namespace
{
    unsigned int sshLength(const std::string& value)
    {
        if (value.size() > static_cast<size_t>((std::numeric_limits<unsigned int>::max)()))
        {
            return 0U;
        }

        return static_cast<unsigned int>(value.size());
    }
}

SFTPUploader::SFTPUploader(const string& ip, const string& user, const string& pass)
    : m_sRemoteIP(ip)
    , m_sUsername(user)
    , m_sPassword(pass)
    , m_Sock(INVALID_SOCKET)
    , session(nullptr)
    , sftp_session(nullptr)
{
    if (!Connect())
    {
        std::cerr << "Unable to connect to host: " << m_sRemoteIP << " with user: " << m_sUsername << "\n";
    }
}

SFTPUploader::~SFTPUploader()
{
    if (sftp_session)
    {
        libssh2_sftp_shutdown(sftp_session);
    }

    if (session)
    {
        libssh2_session_disconnect(session, "Normal Shutdown");
        libssh2_session_free(session);
    }

    if (m_Sock != INVALID_SOCKET)
    {
        closesocket(m_Sock);
    }

    WSACleanup();
    libssh2_exit();
}

bool SFTPUploader::Connect()
{
    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0)
    {
        std::cerr << "WSAStartup failed.\n";
        return false;
    }

    struct sockaddr_in sin;
    m_Sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_Sock == INVALID_SOCKET)
    {
        std::cerr << "Unable to open socket.\n";
        return false;
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    inet_pton(AF_INET, m_sRemoteIP.c_str(), &sin.sin_addr);

    if (connect(m_Sock, (struct sockaddr*)(&sin), sizeof(sin)) != 0)
    {
        std::cerr << "Failed to connect to SSH: " << m_sRemoteIP << "\n";
        closesocket(m_Sock);
        return false;
    }

    libssh2_init(0);
    session = libssh2_session_init();
    if (!session)
    {
        std::cerr << "Unable to initiate SSH session.\n";
        return false;
    }

    if (libssh2_session_handshake(session, m_Sock))
    {
        std::cerr << "Unable to establish SSH handshake.\n";
        return false;
    }

    std::cout << "Connecting to " << m_sRemoteIP << " as " << m_sUsername << "\n";

    if (libssh2_userauth_password(session, m_sUsername.c_str(), m_sPassword.c_str()))
    {
        std::cerr << "Authentication failed for: " << m_sRemoteIP << "\n";
        return false;
    }

    sftp_session = libssh2_sftp_init(session);
    if (!sftp_session)
    {
        std::cerr << "Unable to initiate SFTP session for: " << m_sRemoteIP << "\n";
        return false;
    }

    return true;
}

bool SFTPUploader::UploadFile(const string& localFile, const string& remoteFile)
{
    ifstream file(localFile, ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Unable to open local file: " << localFile << "\n";
        return false;
    }

    const unsigned int remotePathLength = sshLength(remoteFile);
    if (remotePathLength == 0U)
    {
        std::cerr << "Invalid remote file path: " << remoteFile << "\n";
        return false;
    }

    LIBSSH2_SFTP_HANDLE* sftp_handle = libssh2_sftp_open_ex(sftp_session, remoteFile.c_str(), remotePathLength,
        LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
        LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
        LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH,
        LIBSSH2_SFTP_OPENFILE);
    if (!sftp_handle)
    {
        std::cerr << "Unable to open remote file: " << remoteFile << "\n";
        return false;
    }

    char buffer[4096];
    ssize_t bytesWritten;
    while (file.read(buffer, sizeof(buffer)))
    {
        bytesWritten = libssh2_sftp_write(sftp_handle, buffer, file.gcount());
        if (bytesWritten < 0)
        {
            std::cerr << "Failed to write to remote file.\n";
            libssh2_sftp_close(sftp_handle);
            return false;
        }
    }

    libssh2_sftp_close(sftp_handle);
    return true;
}

bool SFTPUploader::DownloadFile(const string& remoteFile, const string& localFile)
{
    if (!sftp_session)
    {
        std::cerr << "SFTP session is not initialized.\n";
        return false;
    }

    const unsigned int remotePathLength = sshLength(remoteFile);
    if (remotePathLength == 0U)
    {
        std::cerr << "Invalid remote file path: " << remoteFile << "\n";
        return false;
    }

    LIBSSH2_SFTP_HANDLE* sftp_handle = libssh2_sftp_open_ex(sftp_session, remoteFile.c_str(), remotePathLength, LIBSSH2_FXF_READ, 0, LIBSSH2_SFTP_OPENFILE);
    if (!sftp_handle)
    {
        std::cerr << "Failed to open remote file: " << remoteFile << "\n";
        return false;
    }

    std::ofstream localFileStream(localFile, std::ios::binary);
    if (!localFileStream)
    {
        std::cerr << "Failed to open local file for writing: " << localFile << "\n";
        libssh2_sftp_close(sftp_handle);
        return false;
    }

    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = libssh2_sftp_read(sftp_handle, buffer, sizeof(buffer))) > 0)
    {
        localFileStream.write(buffer, bytesRead);
        if (!localFileStream)
        {
            std::cerr << "Failed to write to local file.\n";
            libssh2_sftp_close(sftp_handle);
            return false;
        }
    }

    localFileStream.close();
    libssh2_sftp_close(sftp_handle);

    return bytesRead >= 0;
}

bool SFTPUploader::ExecuteCommand(const string& command)
{
    LIBSSH2_CHANNEL* channel = libssh2_channel_open_session(session);
    if (!channel)
    {
        std::cerr << "Unable to open channel session.\n";
        return false;
    }

    if (libssh2_channel_exec(channel, command.c_str()))
    {
        std::cerr << "Unable to execute command: " << command << "\n";
        libssh2_channel_close(channel);
        libssh2_channel_free(channel);
        return false;
    }

    char buffer[1024];
    ssize_t n;
    while ((n = libssh2_channel_read(channel, buffer, sizeof(buffer))) > 0)
    {
        std::cout << "Command output: " << string(buffer, n);
    }

    libssh2_channel_close(channel);
    libssh2_channel_free(channel);

    return true;
}

bool SFTPUploader::CleanupRemoteFile(const string& remoteFile)
{
    const unsigned int remotePathLength = sshLength(remoteFile);
    if (remotePathLength == 0U)
    {
        std::cerr << "Invalid remote file path: " << remoteFile << "\n";
        return false;
    }

    return libssh2_sftp_unlink_ex(sftp_session, remoteFile.c_str(), remotePathLength) == 0;
}
