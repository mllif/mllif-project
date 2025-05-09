using Microsoft.CodeAnalysis;
using MLLIFCSharpFront;
using MLLIFCSharpFrontBuild.Diagnostics;

namespace MLLIFCSharpFrontBuild.Serialization;

public readonly struct MethodDefWriter(IMethodSymbol symbol) : ICodeWriter
{
    public IEnumerable<WorkspaceDiagnostic> WriteTo(CodeWriter w, CodeContext ctx)
    {
        if (!symbol.IsTarget())
            yield break;
        
        var intParms    = symbol.Parameters.Select(parm => $"{parm.Type.ToNativeInterfaceType()} {parm.Name}");
        var intParmsStr = string.Join(", ", intParms);

        var brParms = symbol.Parameters.Select(parm => $"{parm.Type.ToNativeBridgeType()} {parm.Name}");
        if (!symbol.IsStatic)
            brParms = brParms.Prepend("void*");
        var brParmsStr = string.Join(", ", brParms);

        var args = symbol.Parameters.Select(parm => parm.Name);
        if (!symbol.IsStatic)
            args = args.Prepend("_handle");
        var argsStr = string.Join(", ", args);

        var delegateName = $"__delegate{symbol.MangleName()}";
        var delegateSymbol = symbol.ContainingType.GetMembers().SingleOrDefault(t => t.Name == delegateName);
        if (delegateSymbol is not ITypeSymbol delegateType)
        {
            yield return BindingDiagnostic.Error(
                ctx.Project, symbol,
                $"error: corresponded delegate type of {symbol.ToDisplayString()} cannot be found");
            yield break;
        }

        if (symbol.ReturnsVoid)
            w.Write("void ");
        else
        {
            w.Write(symbol.ReturnType.ToNativeInterfaceType(true));
            w.Write(' ');
        }

        w.WriteLine(
            $$"""
              {{symbol.ContainingType.GetFullName("::")}}::{{symbol.Name}}({{intParmsStr}}) {
                static {{symbol.ReturnType.ToNativeBridgeType()}} (*__proxy)({{brParmsStr}}) = nullptr;
                if (!__proxy) {
                  ::s_get_function_pointer("{{symbol.ContainingType.GetFullyQualifiedName()}}", "{{symbol.MangleName()}}", "{{delegateType.GetFullyQualifiedName()}}", nullptr, nullptr, reinterpret_cast<void**>(&__proxy));
                }
              """);
        w.Indent++;

        if (!symbol.ReturnsVoid)
            w.Write("return ");
        w.WriteLine($"__proxy({argsStr});");

        w.Indent--;

        w.WriteLine("}");
        w.WriteLine();
    }
}