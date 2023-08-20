
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using HWND = System.IntPtr;
using System;
using System.Security.Cryptography.X509Certificates;
using System.IO.Pipes;



namespace PsiService
{

    public class Worker : BackgroundService
    {
        const int WINDOW_SCAN_DELAY_MS = 250;
        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            [DllImport("user32.dll", SetLastError = true)]
            static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint processId);
            [DllImport("user32.dll")]
            static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

            [DllImport("Kernel32")]
            static extern IntPtr GetConsoleWindow();

            const int SW_HIDE = 0;
            const int SW_SHOW = 5;

            IntPtr hwndMain;
            hwndMain = GetConsoleWindow();
            ShowWindow(hwndMain, SW_HIDE);

            IntPtr handle;
            string title;
            X509Certificate cert2;
            uint processIDu;

            while (!stoppingToken.IsCancellationRequested)
            {
                foreach (KeyValuePair<IntPtr, string> window in OpenWindowGetter.GetOpenWindows())
                {
                    handle = window.Key;
                    title = window.Value;                  
                    processIDu = 0;
                    _ = GetWindowThreadProcessId(handle, out processIDu);

                    int processIDint = (int)processIDu;

                    Process localByName = Process.GetProcessById(processIDint);
                    try
                    {
                        if (localByName is not null && localByName.MainModule is not null && localByName.MainModule.FileName is not null)
                        {
                            cert2 = X509Certificate.CreateFromSignedFile(localByName.MainModule.FileName);
                            if (cert2 != null)
                            {
                                if (cert2.Subject.Contains("psiphon", StringComparison.OrdinalIgnoreCase))
                                {
                                    localByName.CloseMainWindow();
                                }
                            }
                        }
                    }
                    catch
                    {

                    }
                }
                await Task.Delay(WINDOW_SCAN_DELAY_MS, stoppingToken);
            }
        }
    }
}

public static class OpenWindowGetter
{
    /// <summary>Returns processIDint dictionary that contains the handle and title of all the open windows.</summary>
    /// <returns>A dictionary that contains the handle and title of all the open windows.</returns>
    public static IDictionary<HWND, string> GetOpenWindows()
    {
        HWND shellWindow = GetShellWindow();
        Dictionary<HWND, string> windows = new Dictionary<HWND, string>();

        EnumWindows(delegate (HWND hWnd, int lParam)
        {
            if (hWnd == shellWindow) return true;
            if (!IsWindowVisible(hWnd)) return true;

            int length = GetWindowTextLength(hWnd);
            if (length == 0) return true;

            StringBuilder builder = new StringBuilder(length);
            GetWindowText(hWnd, builder, length + 1);

            windows[hWnd] = builder.ToString();
            return true;

        }, 0);

        return windows;
    }

    private delegate bool EnumWindowsProc(HWND hWnd, int lParam);

    [DllImport("USER32.DLL")]
    private static extern bool EnumWindows(EnumWindowsProc enumFunc, int lParam);

    [DllImport("USER32.DLL")]
    private static extern int GetWindowText(HWND hWnd, StringBuilder lpString, int nMaxCount);

    [DllImport("USER32.DLL")]
    private static extern int GetWindowTextLength(HWND hWnd);

    [DllImport("USER32.DLL")]
    private static extern bool IsWindowVisible(HWND hWnd);

    [DllImport("USER32.DLL")]
    private static extern IntPtr GetShellWindow();

}
