using System.CodeDom.Compiler;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.MSBuild;
using MLLIFCSharpFrontBuild.Diagnostics;

namespace MLLIFCSharpFrontBuild;

internal static class Program
{
    private static IGenerator[] _generators = [ new HeaderGenerator(), new SourceGenerator() ];
    
    private static async Task<int> Main(string[] args)
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

        Project project;

        try
        {
            project = await msbuild.OpenProjectAsync(projectPath);
        }
        catch (FileNotFoundException)
        {
            Console.Error.WriteLine($"error: no such project '{projectPath}'");
            return -1;
        }

        var start = DateTime.Now;
        
        var compilation = await project
            .WithCompilationOptions(compileArgs.CompilationOptions)
            .GetCompilationAsync();
        
        var elapsed = DateTime.Now - start;
        Console.Error.WriteLine($"info: {elapsed.TotalSeconds:F1}s taken to build");
        
        if (compilation is null)
        {
            Console.Error.WriteLine("error: failed to build project");
            return -1;
        }

        await using var dest   = new StringWriter();
        await using var writer = new IndentedTextWriter(dest, "  ");

        var ns = compilation.Assembly.GlobalNamespace;
        
        foreach (var generator in _generators)
        {
            var diags = generator.Run(writer, ns, project).ToArray();
            if (diags.Length == 0)
                continue;
            
            diags.PrintDiagnostics();
            return -1;
        }

        return 0;
    }
}