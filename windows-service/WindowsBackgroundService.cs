using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using static App.WindowsService.ProcessHandler;
using HWND = System.IntPtr;
using System.IO.Pipes;
using System.Threading;

namespace App.WindowsService;

public sealed class WindowsBackgroundService : BackgroundService
{
    private readonly AppGuardService _appGuardService;
    private readonly ILogger<WindowsBackgroundService> _logger;

    public WindowsBackgroundService(
        AppGuardService appGuardService,
        ILogger<WindowsBackgroundService> logger) =>
        (_appGuardService, _logger) = (appGuardService, logger);

    [DllImport("user32.dll", SetLastError = true)]
    static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint processId);

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        try
        {          
            _logger.LogWarning("Hello world. Starting service {time}", DateTimeOffset.Now);
            string executablePath = Path.Combine(AppContext.BaseDirectory, "PsiService.exe");
            bool success;
            while (!stoppingToken.IsCancellationRequested)
            {              
                Process[] ps = Process.GetProcessesByName("explorer");
                for (int i = 0; i < ps.Length; i++)
                {                       
                    success = ProcessHandler.ProcessAsUser.Launch(executablePath, ps[i].SessionId);
                    if(success)
                    {
                        _logger.LogWarning(i.ToString() + " Started PsiService in Session ID: " + ps[i].SessionId.ToString());
                    }
                }                                         
                await Task.Delay(TimeSpan.FromSeconds(5f), stoppingToken);                
            }
        }
        catch (TaskCanceledException)
        {
            // When the stopping token is canceled, for example, a call made from services.msc,
            // we shouldn't exit with a non-zero exit code. In other words, this is expected...
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "{Message}", ex.Message);

            // Terminates this process and returns an exit code to the operating system.
            // This is required to avoid the 'BackgroundServiceExceptionBehavior', which
            // performs one of two scenarios:
            // 1. When set to "Ignore": will do nothing at all, errors cause zombie services.
            // 2. When set to "StopHost": will cleanly stop the host, and log errors.
            //
            // In order for the Windows Service Management system to leverage configured
            // recovery options, we need to terminate the process with a non-zero exit code.
            Environment.Exit(1);
        }
    }
}