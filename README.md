# DesktopProcessMonitor

Project has 3 sub-projects:
1. App.Windows.Service: C# Windows Service. Spawns WMProcMon for each logged in user. Re-runs WMProcMon if it is terminated by the user. 
2. WMProcMon: C++ background worker. Subscribes to the _InstanceCreationEvent_ message via WMI. This event is a signal that a new process was created _for_ or _by_ the user. 
    WMProcMon looksup the Authenticode certificate for the exe_file of each new process. If the publisher matches "Psiphon Inc, WMProcMon terminates the processes. 
    Note: this is a non-blocking operation, no processes on the OS will be prevented or delayed from starting-up. However the process will terminate within a few ms if its certificate publisher is a match to the target string. 
3. SetPsiBlock: The Visual Studio Setup project handling installation and uninstallation of App.Windows.Service and WMProcMon.

# Security considerations

Resisting Priv-Esc tecniques:
1. The file path to service executable is quoted.
2. Service executable folder is read-only for unpriveliged users.
3. WMProcMon executable folder is read-only for unpriveliged users.
4. WMProcMon runs with the owner set as the current logged-in user or users.

