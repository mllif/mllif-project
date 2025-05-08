using Microsoft.CodeAnalysis;
using MLLIFCSharpFront;

namespace MLLIFCSharpFrontBuild.Serialization;

public readonly ref struct NamespaceWriter(INamespaceSymbol symbol) : ICodeWriter
{
    public void WriteTo(CodeWriter w, CodeContext ctx)
    {
        if (!symbol.IsTarget())
            return;

        if (symbol.IsGlobalNamespace)
        {
            w.WriteLine(
                """
                #pragma once
                
                #include <cstdint>
                
                """);
        }
        else
        {
            w.WriteLine($"namespace {symbol.Name} {{");
            w.Indent++;
        }

        foreach (var ns in symbol.GetNamespaceMembers()) 
            new NamespaceWriter(ns).WriteTo(w, ctx);
        
        foreach (var t in symbol.GetTypeMembers().Where(SymbolExtension.IsTarget))
            new TypeWriter(t).WriteTo(w, ctx);

        if (symbol.IsGlobalNamespace)
        {
            w.WriteLine(
                """
                
                namespace mllif {
                #if _WIN32
                  int init(const wchar_t *runtimeConfigPath, const wchar_t *assemblyPath);
                #else
                  int init(const char *runtimeConfigPath, const char *assemblyPath);
                #endif
                  void close();
                }
                """);
        }
        else
        {
            w.Indent--;
            w.WriteLine("}");
        }
    }
}