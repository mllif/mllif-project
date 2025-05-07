using System.CodeDom.Compiler;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.MSBuild;

namespace MLLIFCSharpFrontBuild;

internal static class Program
{
    static async Task<int> Main(string[] args)
    {
        using var msbuild = MSBuildWorkspace.Create();

        if (args.Length < 1)
        {
            Console.Error.WriteLine(
                "error: insufficient arguments\n" +
                "usage: MLLIFCSharpFrontBuild <csproj-file> <compile-args...>");
            return -1;
        }

        var projectPath = args[0];
        var compileArgsStr = args.Skip(1);
        var compileArgs = CSharpCommandLineParser.Default.Parse(compileArgsStr, Environment.CurrentDirectory, null);

        var project = await msbuild.OpenProjectAsync(projectPath);
        
        var compilation = await project
            .WithCompilationOptions(compileArgs.CompilationOptions)
            .GetCompilationAsync();
        if (compilation is null)
        {
            Console.Error.WriteLine("compilation is null");
            return -1;
        }

        using var dest   = new StringWriter();
        using var writer = new IndentedTextWriter(dest, "  ");

        var ns = compilation.Assembly.GlobalNamespace;
        new HeaderGenerator().Run(writer, ns, project);
        new SourceGenerator().Run(writer, ns, project);

        return 0;
    }
}