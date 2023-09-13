# DesktopProcessMonitor
The purpose of this Winodows 10/11 application is to terminate policy-prohibited applications on corportate-owned devices. This application is intended for system adminstrators to install on managed devices. An example of an prohibited application could be a 3rd party VPN binary such as _Psiphon 3_. The default behavior is **DesktopProcessMonitor** is to terminate Psiphon 3, however the code can be simply modified to target any policy-prohibitied applications.

# Why not use Group Policy Editor?

Group Policy Editor on Windows 10/11 blocks processes from starting-up until it has checked the binary's certificate; significantly and noticibly impacting system performance, especially on older hardware. 
DesktopProcessMonitor does not block processes from starting up. It does however terminate new processes quickly if their certificates matche any policy-prohibited publisher. This results in no noticible impact to system performance.

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
4. WMProcMon runs with the owner set as the current logged-in user or users.

