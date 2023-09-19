# Download Latest

https://github.com/xyz2022/DesktopProcessMonitor/releases/latest

# DesktopProcessMonitor
Terminate policy-prohibited applications on corportate-owned devices. This application aims to block the use of specific apps on managed devices. Example: 3rd party VPN binaries like _Psiphon 3_ may be against the terms of use for users of managed devices, however the users have been using the VPN despite comnay policy. In this case, the default behavior of **DesktopProcessMonitor** is to terminate Psiphon 3 when the user tries to open it. The default behavior can be modified to target any policy-prohibitied applications.

# Why not Group Policy Editor?

Slowdown. For each process, gpedit blocks processes from starting-up until the certificate is located and read; impacting system performance, especially on older hardware. On the other hand, DesktopProcessMonitor is fast. It does not block processes while certifcates are located and read. It does however terminate new processes if their certificates are from any policy-prohibited publisher. This results in reduced impact to system performance.

![image](https://github.com/xyz2022/DesktopProcessMonitor/assets/91395091/d90723cf-b489-490a-9daa-77ee7e76da04)

# Architecture

Project has 3 sub-projects:
1. App.Windows.Service: C# Windows Service. Spawns WMProcMon for each logged in user. Re-runs WMProcMon if it is terminated by the user. 
2. WMProcMon: C++ background worker. Subscribes to the _InstanceCreationEvent_ message via WMI. This event is a signal that a new process was created _for_ or _by_ the user. 
    WMProcMon looksup the Authenticode certificate for the exe_file of each new process. If the publisher matches "Psiphon Inc", WMProcMon terminates the process. 
    Note: this is a non-blocking operation, no processes on the OS will be prevented or delayed from starting-up. However the process will terminate within a few ms if its certificate publisher is a match to the target string. 
3. SetPsiBlock: The Visual Studio Setup project handling installation and uninstallation of App.Windows.Service and WMProcMon.

# Security considerations

Resisting Priv-Esc tecniques:
1. The file path to service executable is quoted.
2. Service executable folder is read-only for unpriveliged users.
3. WMProcMon executable folder is read-only for unpriveliged users.
4. WMProcMon process is owned by the current logged-in user.

# How-to Build 

1. Open _workers.sln_ in Microsoft Visual Studio 2022 17.7.4 or higher
3. Publish App.Windows.Service  -> Local Folder (use default selected folder)-> Enabled Options: Self-contained, Produce Single File, EnableReadyToRunCompilation
4. Rebuild WMProcMon
5. Rebuild SetupPsiBlock
6. SetupPsiBlock.msi will build in ./release/x64 directory

# Install
Desktop Users install via MSI

Network administrators install & uninstall silently via script:

Install ->   
    
    msiexec.exe /i SetupPsiBlock.msi MSIFASTINSTALL=4 /qn /norestart

Uninstall ->

    msiexec.exe /x SetupPsiBlock.msi MSIFASTINSTALL=4 /qn /norestart
    
