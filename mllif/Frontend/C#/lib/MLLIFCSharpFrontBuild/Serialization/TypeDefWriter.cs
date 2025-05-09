using Microsoft.CodeAnalysis;
using MLLIFCSharpFront;
using MLLIFCSharpFrontBuild.Diagnostics;

namespace MLLIFCSharpFrontBuild.Serialization;

public readonly struct TypeDefWriter(INamedTypeSymbol symbol) : ICodeWriter
{
    public IEnumerable<WorkspaceDiagnostic> WriteTo(CodeWriter w, CodeContext ctx)
    {
        if (!symbol.IsTarget())
            yield break;

        if (!symbol.IsStatic)
        {
            var internals = symbol
                .ContainingAssembly
                .GlobalNamespace
                .GetAllTypes()
                .FirstOrDefault(type => type.GetFullName() == "MLLIF.Internal");
            var freeDelegate = symbol
                .ContainingAssembly
                .GlobalNamespace
                .GetAllTypes()
                .FirstOrDefault(type => type.GetFullName() == "MLLIF.Internal.FreeDelegate");
            if (internals is null || freeDelegate is null)
            {
                yield return BindingDiagnostic.Error(ctx.Project, symbol, "couldn't find embedded internals; is source generator added to project?");
            }
            else
            {
                w.WriteLine($"{symbol.GetFullName("::")}::~{symbol.Name}() {{");
                w.Indent++;
                w.WriteLine(
                    $$"""
                      static void (*__proxy)(void*) = nullptr;
                      if (!__proxy) {
                        ::s_get_function_pointer("{{internals.GetFullyQualifiedName()}}", "{{internals.MetadataName}}", "{{freeDelegate.GetFullyQualifiedName()}}", nullptr, nullptr, reinterpret_cast<void**>(&__proxy));
                      }
                      __proxy(_handle);
                      """);
                w.Indent--;
                w.WriteLine("}");
                w.WriteLine();
            }
        }
        
        var methods = symbol
            .GetMembers()
            .OfType<IMethodSymbol>()
            .Where(SymbolExtension.IsTarget)
            .ToArray();
        foreach (var method in methods)
        foreach (var diag in new MethodDefWriter(method).WriteTo(w, ctx))
            yield return diag;
    }
}