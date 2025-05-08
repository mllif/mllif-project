using System.CodeDom.Compiler;
using System.Text;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.MSBuild;
using MLLIFCSharpFrontBuild.Diagnostics;
using MLLIFCSharpFrontBuild.Serialization;
using Humanizer;

namespace MLLIFCSharpFrontBuild;

internal static class Program
{
    private static async Task<int> Main(string[] args)
    {
        using var msbuild = MSBuildWorkspace.Create();

        if (args.Length < 1)
        {
            Console.Error.WriteLine(
                "error: insufficient arguments\n" +
                "usage: MLLIFCSharpFrontBuild <csproj-file> <output-dir> <compile-args...>");
            return -1;
        }

        var projectPath    = args[0];
        var outputDir    = args[1];
        var compileArgsStr = args.Skip(2);
        var compileArgs    = CSharpCommandLineParser.Default.Parse(compileArgsStr, Environment.CurrentDirectory, null);

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

            compilation = await project
                .WithCompilationOptions(compileArgs.CompilationOptions)
                .GetCompilationAsync();

            var elapsed = DateTime.Now - start;
            Console.Error.WriteLine($"info: {elapsed.Humanize()} taken to build");
        }

        if (compilation is null)
        {
            Console.Error.WriteLine("error: failed to build project");
            return -1;
        }

        Directory.CreateDirectory(outputDir);

        await using var hdr  = new StreamWriter(File.OpenWrite(Path.Combine(outputDir, "library.h")), Encoding.UTF8);
        await using var src  = new StreamWriter(File.OpenWrite(Path.Combine(outputDir, "library.cxx")), Encoding.UTF8);
        await using var hdrW = new CodeWriter(hdr, "  ");
        await using var srcW = new CodeWriter(src, "  ");
        
        // Write bindings
        {
            var start = DateTime.Now;

            var ns = compilation.Assembly.GlobalNamespace;
            var ctx = new CodeContext();
            
            new NamespaceWriter(ns).WriteTo(hdrW, ctx);
            
            srcW.WriteLine(
                """
                #include "library.h"
                
                #include <hostfxr.h>
                #include <coreclr_delegates.h>
                
                #if _WIN32
                #  include <libloaderapi.h>
                #  define c_dlopen LoadLibraryW
                #  define c_dlclose FreeLibrary
                #  define c_dlsym GetProcAddress
                #else
                #  include <dlfcn.h>
                #  define c_dlopen(f) dlopen(f, RTLD_LAZY | RTLD_LOCAL)
                #  define c_dlclose dlclose
                #  define c_dlsym dlsym
                #endif
                
                static get_function_pointer_fn __s_get_function_pointer_fn;
                static void* __s_dl;
                static hostfxr_handle __s_hostfxr;
                
                int mllif::init(const char_t *runtimeConfigPath, const char_t *assemblyPath) {
                  static char_t buffer[8192];
                    
                  size_t size = sizeof buffer;
                  if (get_hostfxr_path(buffer, &size, nullptr)) {
                    return -1;
                  }
                  
                  __s_dl = c_dlopen(buffer);
                  
                  auto initr = (hostfxr_initialize_for_runtime_config_fn)c_dlsym(__s_dl, "hostfxr_initialize_for_runtime_config");
                  if (!initr) return -2;
                  
                  auto get_delegate = (hostfxr_get_runtime_delegate_fn)c_dlsym(__s_dl, "hostfxr_get_runtime_delegate");
                  if (!get_delegate) return -3;
                  
                  hostfxr_handle __s_hostfxr;
                  if (initr(runtimeConfigPath, nullptr, &__s_hostfxr)) return -4;
                  
                  load_assembly_fn load_assembly;
                  if (get_delegate(__s_hostfxr, hdt_load_assembly, (void**)&load_assembly))
                    return -5;
                  if (load_assembly(assemblyPath, nullptr, nullptr))
                    return -6;
                  
                  if (get_delegate(__s_hostfxr, hdt_get_function_pointer, (void**)&__s_get_function_pointer_fn))
                    return -7;
                  
                  return 0;
                }
                
                void mllif::close() {
                  hostfxr_close(__s_hostfxr);
                  c_dlclose(__s_dl);
                }
                
                """);
            foreach (var method in ctx)
                new MethodDefWriter(method).WriteTo(srcW, ctx);

            var elapsed = DateTime.Now - start;
            Console.Error.WriteLine($"info: {elapsed.Humanize()} taken to generate");
        }

        return 0;
    }
}