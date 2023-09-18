using App.WindowsService;
using Microsoft.Extensions.Logging.Configuration;
using Microsoft.Extensions.Logging.EventLog;
using CliWrap;
using static App.WindowsService.ProcessHandler;
using CliWrap.EventStream;
using System.Diagnostics;

const string ServiceName = "NSW DoE AppGuard";

if (args is { Length: >= 1 })
{
    try
    {
        string executablePath =
            Path.Combine(AppContext.BaseDirectory, "App.WindowsService.exe");

        if (args[0] is "/Install")
        {
            ProcessHandler.ProcessAsUser.DoQuitViaPipe();
            
            await Cli.Wrap("sc")
                .WithArguments(new[] { "create", ServiceName, $"binPath=\"{executablePath}\"", "start=auto" })
                .ExecuteAsync();

            await Cli.Wrap("sc")
                .WithArguments(new[] { "start", ServiceName })
                .ExecuteAsync();
        }
        else if (args[0] is "/Uninstall")
        {
            //Testing alternative to Cli.Wrap
            /*Process p = Process.Start("sc.exe", "stop " + "\"" + ServiceName + "\"");
            p.WaitForExit();
            Process p2 = Process.Start("sc.exe", "delete " + "\"" + ServiceName + "\"");
            p2.WaitForExit();*/

            
            await Cli.Wrap("sc")
                .WithArguments(new[] { "stop", ServiceName })
                .ExecuteAsync(); 

            
            await Cli.Wrap("sc")
                 .WithArguments(new[] { "delete", ServiceName })
                 .ExecuteAsync(); 

            //Send Quit Message to AppGuard processes
            ProcessHandler.ProcessAsUser.DoQuitViaPipe();

            Environment.Exit(0);
        }
    }
    catch (Exception)
    {
        //Console.WriteLine(ex);
    }

    return;
}


HostApplicationBuilder builder = Host.CreateApplicationBuilder(args);
builder.Services.AddWindowsService(options =>
{
    options.ServiceName = ServiceName;
});

LoggerProviderOptions.RegisterProviderOptions<
    EventLogSettings, EventLogLoggerProvider>(builder.Services);

builder.Services.AddSingleton<AppGuardService>();
builder.Services.AddHostedService<WindowsBackgroundService>();

// See: https://github.com/dotnet/runtime/issues/47303
builder.Logging.AddConfiguration(
    builder.Configuration.GetSection("Logging"));

IHost host = builder.Build();
host.Run();

