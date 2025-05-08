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
                #include <hostfxr.h>
                #include <coreclr_delegates.h>
                
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
            w.WriteLine();
            foreach (var method in ctx)
                new MethodDefWriter(method).WriteTo(w, ctx);
        }
        else
        {
            w.Indent--;
            w.WriteLine("}");
        }
    }
}