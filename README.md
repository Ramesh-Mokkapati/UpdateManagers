# UpdateManagers

A pair of Windows console utilities for managing software updates across machines.

| Project | Executable | Purpose |
|---|---|---|
| **UpdateManager-Push** | `UpdateManager-Push.exe` | Pushes (uploads) an artifact to a remote host over SSH/SFTP |
| **UpdateManager-Pull** | `UpdateManager-Pull.exe` | Pulls (downloads) an update from a URL and installs it locally |

---

## UpdateManager-Push

Connects to a remote host over SSH and uploads a local file to a specified remote path using SFTP.

### Usage

```
UpdateManager-Push.exe
    --artifact-source <local-path>
    --artifact-target <remote-path>
    --host-ip <ip>
    --host-user <username>
    --host-pass <password>
```

### Parameters

| Parameter | Required | Description |
|---|---|---|
| `--artifact-source` | Yes | Full path to the local file to upload |
| `--artifact-target` | Yes | Full path on the remote host to write the file to |
| `--host-ip` | Yes | IP address of the remote host |
| `--host-user` | Yes | SSH username |
| `--host-pass` | Yes | SSH password |

### Example

```
UpdateManager-Push.exe --artifact-source C:\Build\App.msi --artifact-target /opt/deploy/App.msi --host-ip 10.100.11.25 --host-user admin --host-pass secret
```

### Source Files

| File | Description |
|---|---|
| `Deployer/Deployer.cpp` | Entry point; parses command-line arguments |
| `Deployer/PushManager.h/.cpp` | Orchestrates the upload operation |
| `Deployer/SFTPUploader.h/.cpp` | SSH connection, authentication, and SFTP file transfer via libssh2 |

### Dependencies

- [libssh2](https://www.libssh2.org/) (`libssh2-vc141_xp.1.8.2` NuGet package)
- `Ws2_32.lib` (Windows Sockets)

---

## UpdateManager-Pull

Checks whether a remote update is newer than the locally installed version, downloads it via HTTP/HTTPS, and optionally executes the installer.

### Usage

```
UpdateManager-Pull.exe
    --update-url <url>
    --version-url <url>
    --output-file <local-path>
    --version-file <local-path>
    --target-version <version>
    [--username <username>]
    [--password <password>]
    [--download-only]
```

### Parameters

| Parameter | Required | Description |
|---|---|---|
| `--update-url` | Yes | URL to download the update artifact from |
| `--version-url` | Yes | URL to check the remote version |
| `--output-file` | Yes | Local path to save the downloaded artifact |
| `--version-file` | Yes | Local file that stores the currently installed version |
| `--target-version` | Yes | Version string to update to |
| `--username` | No | Credential for authenticated downloads |
| `--password` | No | Credential for authenticated downloads |
| `--download-only` | No | Download the artifact but skip execution |

### Example

```
UpdateManager-Pull.exe --update-url http://builds.internal/App.msi --version-url http://builds.internal/version.txt --output-file C:\Updates\App.msi --version-file C:\Install\version.txt --target-version 2.1.0
```

### Source Files

| File | Description |
|---|---|
| `Updater/PullManager.cpp` | Entry point, `PullManager` class, and all download/install logic |

### Dependencies

- `wininet.lib` (Windows Internet)

---

## Building

Open `UpdateManagers.sln` in Visual Studio 2022 and build the desired configuration (`Debug|x64` or `Release|x64`). Both projects target Windows 10 and use the v143 toolset.
