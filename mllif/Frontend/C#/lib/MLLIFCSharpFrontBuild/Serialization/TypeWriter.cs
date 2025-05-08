using Microsoft.CodeAnalysis;
using MLLIFCSharpFront;

namespace MLLIFCSharpFrontBuild.Serialization;

public readonly ref struct TypeWriter(INamedTypeSymbol symbol) : ICodeWriter
{
    public void WriteTo(CodeWriter w, CodeContext ctx)
    {
        if (!symbol.IsTarget())
            return;
        
        w.WriteLine(
            $$"""
            class [[mllif::export]] {{symbol.Name}} {
              void* _handle;
            public:
              {{symbol.Name}}({{symbol.Name}} const&) = delete;
              {{symbol.Name}}& operator=({{symbol.Name}} const&) = delete;
            """);
        w.Indent++;

        var methods = symbol
            .GetMembers()
            .OfType<IMethodSymbol>()
            .Where(SymbolExtension.IsTarget)
            .ToArray();
        ctx.AddRange(methods);
        foreach (var method in methods)
        {
            new MethodDeclWriter(method).WriteTo(w, ctx);
        }

        w.Indent--;
        w.WriteLine("};");
    }
}