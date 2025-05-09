using Microsoft.CodeAnalysis;
using MLLIFCSharpFront;

namespace MLLIFCSharpFrontBuild.Serialization;

public readonly struct MethodDeclWriter(IMethodSymbol symbol) : ICodeWriter
{
    public IEnumerable<WorkspaceDiagnostic> WriteTo(CodeWriter w, CodeContext ctx)
    {
        if (!symbol.IsTarget())
            yield break;
        
        if (symbol.IsStatic)
            w.Write("static ");
        
        if (symbol.ReturnsVoid)
            w.Write("void ");
        else
        {
            w.Write(symbol.ReturnType.ToNativeInterfaceType(true));
            w.Write(' ');
        }

        var intParms = symbol.Parameters.Select(parm => $"{parm.Type.ToNativeInterfaceType()} {parm.Name}");
        var intParmsStr = string.Join(", ", intParms);
        
        w.WriteLine($"{symbol.Name}({intParmsStr});");
    }
}