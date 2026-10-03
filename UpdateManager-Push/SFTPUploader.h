#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include "libssh2.h"
#include "libssh2_sftp.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libssh2.lib")

class SFTPUploader
{
public:
	SFTPUploader(const string& ip, const string& user, const string& pass);

	~SFTPUploader();

	bool UploadFile(const string& localFile, const string& remoteFile);

	bool DownloadFile(const string& remoteFile, const string& localFile);

	bool ExecuteCommand(const string& command);

	bool CleanupRemoteFile(const string& remoteFile);

private:
	string			m_sRemoteIP;
	string			m_sUsername;
	string			m_sPassword;
	SOCKET			m_Sock;
	WSADATA			m_wsaData;

	LIBSSH2_SESSION* session;
	LIBSSH2_SFTP* sftp_session;

	bool Connect();
};
