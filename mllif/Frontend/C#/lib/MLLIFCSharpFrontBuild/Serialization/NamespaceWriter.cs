using Microsoft.CodeAnalysis;
using MLLIFCSharpFront;

namespace MLLIFCSharpFrontBuild.Serialization;

public readonly struct NamespaceWriter(INamespaceSymbol symbol) : ICodeWriter
{
    private static string Stub;
    
    static NamespaceWriter()
    {
        Stub = Embedded.Resource("BridgeStub.h").GetAwaiter().GetResult();
    }
    
    public IEnumerable<WorkspaceDiagnostic> WriteTo(CodeWriter w, CodeContext ctx)
    {
        if (!symbol.IsTarget())
            yield break;

        if (symbol.IsGlobalNamespace)
        {
            w.WriteLine(Stub);
        }
        else
        {
            w.WriteLine($"namespace {symbol.Name} {{");
            w.Indent++;
        }

        foreach (var ns in symbol.GetNamespaceMembers())
        foreach (var diag in new NamespaceWriter(ns).WriteTo(w, ctx))
            yield return diag;

        foreach (var t in symbol.GetTypeMembers().Where(SymbolExtension.IsTarget))
        foreach (var diag in new TypeDeclWriter(t).WriteTo(w, ctx))
            yield return diag;

        if (!symbol.IsGlobalNamespace)
        {
            w.Indent--;
            w.WriteLine("}");
        }
    }
}