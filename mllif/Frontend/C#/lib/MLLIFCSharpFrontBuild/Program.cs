using System.Text;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.MSBuild;
using MLLIFCSharpFrontBuild.Serialization;
using Humanizer;
using Project = Microsoft.CodeAnalysis.Project;

namespace MLLIFCSharpFrontBuild;

internal static class Program
{
    private static async Task<int> Main(string[] args)
    {
        using var msbuild = MSBuildWorkspace.Create();

        if (args.Length < 2)
        {
            Console.Error.WriteLine(
                "error: insufficient arguments\n" +
                "usage: MLLIFCSharpFrontBuild <csproj-file> <output-dir>");
            return -1;
        }

        var projectPath = args[0];
        var outputDir   = args[1];

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

        // Get compilation
        Compilation? compilation;
        {
            var start = DateTime.Now;

            compilation = await project.GetCompilationAsync();

            var elapsed = DateTime.Now - start;
            Console.Error.WriteLine($"info: {elapsed.Humanize()} taken to build");
        }

        if (compilation is null)
        {
            Console.Error.WriteLine("error: failed to build project");
            return -1;
        }

        // Prepare resources for bindings
        Directory.CreateDirectory(outputDir);
        await using var hdr  = new StreamWriter(File.Create(Path.Combine(outputDir, "library.h")), Encoding.UTF8);
        await using var src  = new StreamWriter(File.Create(Path.Combine(outputDir, "library.cxx")), Encoding.UTF8);
        await using var hdrW = new CodeWriter(hdr, "  ");
        await using var srcW = new CodeWriter(src, "  ");
        
        // Write bindings
        var stub = await Embedded.Resource("BridgeStub.cxx");
        {
            var start = DateTime.Now;

            var ns  = compilation.Assembly.GlobalNamespace;
            var ctx = new CodeContext(project);

            foreach (var diag in new NamespaceWriter(ns).WriteTo(hdrW, ctx)) 
                Console.Error.WriteLine(diag);
            
            srcW.WriteLine(stub);
            foreach (var t in ctx)
            foreach (var diag in new TypeDefWriter(t).WriteTo(srcW, ctx))
                Console.Error.WriteLine(diag);

            var elapsed = DateTime.Now - start;
            Console.Error.WriteLine($"info: {elapsed.Humanize()} taken to generate");
        }
        
        

        // Infer runtime version
        string? version = null;
        foreach (var reference in compilation.References)
        {
            if (reference.Display is null)
                continue;

            var terms = reference.Display.Replace("\\", "/").Split('/', StringSplitOptions.RemoveEmptyEntries);

            int i;
            for (i = 0; i < terms.Length; i++)
                if (terms[i] == "microsoft.netcore.app.ref")
                    break;
            i++;
            if (i >= terms.Length)
                continue;

            version = terms[i];
            break;
        }
        if (version is null)
        {
            version = Environment.Version.ToString();
            Console.Error.WriteLine(
                $"warning: cannot infer .net runtime version; defaults to running .net runtime version ({version})");
        }
        
        // Serve CMakeLists.txt
        var cmake = await Embedded.Resource("CMakeLists.txt");
        cmake = cmake.Replace("{@}", version);
        await File.WriteAllTextAsync(Path.Combine(outputDir, "CMakeLists.txt"), cmake);

        return 0;
    }
}