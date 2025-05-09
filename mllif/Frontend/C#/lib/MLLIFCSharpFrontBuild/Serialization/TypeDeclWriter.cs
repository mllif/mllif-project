using Microsoft.CodeAnalysis;
using MLLIFCSharpFront;

namespace MLLIFCSharpFrontBuild.Serialization;

public readonly struct TypeDeclWriter(INamedTypeSymbol symbol) : ICodeWriter
{
    public IEnumerable<WorkspaceDiagnostic> WriteTo(CodeWriter w, CodeContext ctx)
    {
        if (!symbol.IsTarget())
            yield break;
        
        w.WriteLine(
            $$"""
            class [[mllif::export]] {{symbol.Name}} {
              void* _handle;
              {{symbol.Name}}(void* handle) : _handle(handle) {}
            public:
              {{symbol.Name}}({{symbol.Name}} const&) = delete;
              {{symbol.Name}}& operator=({{symbol.Name}} const&) = delete;
            """);
        w.Indent++;

        if (!symbol.IsStatic)
        {
            w.WriteLine($"~{symbol.Name}();");
        }

        ctx.Add(symbol);
        foreach (var method in symbol
                     .GetMembers()
                     .OfType<IMethodSymbol>()
                     .Where(SymbolExtension.IsTarget))
        foreach (var diag in new MethodDeclWriter(method).WriteTo(w, ctx))
            yield return diag;

        w.Indent--;
        w.WriteLine("};");
    }
}