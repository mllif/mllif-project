using Microsoft.CodeAnalysis;
using MLLIFCSharpFront;

namespace MLLIFCSharpFrontBuild.Serialization;

public readonly ref struct MethodDefWriter(IMethodSymbol symbol) : ICodeWriter
{
    public void WriteTo(CodeWriter w, CodeContext ctx)
    {
        if (symbol.IsStatic)
            w.Write("static ");
        
        if (symbol.ReturnsVoid)
            w.Write("void ");
        else
        {
            w.Write(symbol.ReturnType.ToNativeInterfaceType());
            w.Write(' ');
        }

        var intParms = symbol.Parameters.Select(parm => $"{parm.Type.ToNativeInterfaceType()} {parm.Name}");
        var intParmsStr = string.Join(", ", intParms);

        var brParms    = symbol.Parameters.Select(parm => $"{parm.Type.ToNativeBridgeType()} {parm.Name}");
        if (!symbol.IsStatic)
            brParms = brParms.Prepend("void*");
        var brParmsStr = string.Join(", ", brParms);

        var args = symbol.Parameters.Select(parm => parm.Name);
        if (!symbol.IsStatic)
            args = args.Prepend("_handle");
        var argsStr = string.Join(", ", args);

        var delegateName = $"__delegate{symbol.MangleName()}";
        var delegateType = symbol.ContainingType.GetMembers().SingleOrDefault(t => t.Name == delegateName);
        if (delegateType is null)
        {
            Console.Error.WriteLine($"error: corresponded delegate type of {symbol.ToDisplayString()} cannot be found");
            return;
        }
        
        w.WriteLine(
            $$"""
              {{symbol.ContainingType.GetFullName("::")}}::{{symbol.Name}}({{intParmsStr}}) {
                static {{symbol.ReturnType.ToNativeBridgeType()}} (*__proxy)({{brParmsStr}}) = nullptr;
                if (!__proxy) {
                  (decltype(__proxy))::get_function_pointer_fn("{{symbol.ContainingType.MetadataName}}", "{{symbol.Name}}", "{{delegateType.MetadataName}}", nullptr, nullptr, &__proxy);
                }
              """);
        w.Indent++;
        
        if (!symbol.ReturnsVoid)
            w.Write("return ");
        w.WriteLine($"__proxy({argsStr});");
        
        w.Indent--;
        
        w.WriteLine("}");
    }
}