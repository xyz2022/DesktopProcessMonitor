using PsiService;
using CliWrap;
using static System.Net.Mime.MediaTypeNames;
using System.Text;


IHost host = Host.CreateDefaultBuilder(args)
    .ConfigureServices(services =>
    {
        services.AddHostedService<Worker>();
    })
    .Build();


await host.RunAsync();
