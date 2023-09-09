# DesktopProcessMonitor

Project has 3 sub-projects:
1. App.Windows.Service: C# Windows Background Service. Spans WMProcMon for each logged in user. Re-runs WMProcMon if it is terminated by the user. 
2. WMProcMon: C++ console application (invisible). Subscribes to _InstanceCreationEvent_ event messages via WMI. Terminates new processes if their AuthentiCode certificate info matches user-defined blacklist criteria. Non-blocking with no noticable impact on system performance. 
3. SetPsiBlock: The Visual Studio Setup project handling installation and uninstallation of the abobe sub-projects.
