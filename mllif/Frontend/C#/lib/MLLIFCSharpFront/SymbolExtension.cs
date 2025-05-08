using System.Text;
using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFront;

public static class SymbolExtension
{
    public static bool IsTarget(this ISymbol symbol)
    {
        if (symbol is INamespaceOrTypeSymbol t && t.GetMembers().Any(IsTarget))
            return true;
        return symbol.GetAttributes().Any(attr => attr.AttributeClass?.Name == "ExportAttribute");
    }

    public static string GetFullName(this ISymbol symbol, string delimiter = ".")
    {
        var queue = new Stack<string>();
        while (symbol is INamespaceOrTypeSymbol or IMethodSymbol)
        {
            if (!string.IsNullOrWhiteSpace(symbol.Name))
                queue.Push(symbol.Name);
            symbol = symbol.ContainingSymbol;
        }

        var builder = new StringBuilder();
        for (var first = true; queue.Count > 0; first = false)
        {
            if (!first)
                builder.Append(delimiter);
            builder.Append(queue.Pop());
        }

        return builder.ToString();
    }

    private static string? ToMangleCode(this ITypeSymbol type)
    {
        if (type.IsReferenceType)
            return "void*";

        return type.GetFullName() switch
        {
            "System.SByte"  => "s8",
            "System.Int16"  => "s16",
            "System.Int32"  => "s32",
            "System.Int64"  => "s64",
            "System.Byte"   => "u8",
            "System.UInt16" => "u16",
            "System.UInt32" => "u32",
            "System.UInt64" => "u64",
            "System.Half"   => "fp16",
            "System.Single" => "fp32",
            "System.Double" => "fp64",
            _               => null
        };
    }

    public static string ToNativeBridgeType(this ITypeSymbol type)
    {
        return type.GetFullName("::") switch
        {
            "System::SByte"  => "int8_t",
            "System::Int16"  => "int16_t",
            "System::Int32"  => "int32_t",
            "System::Int64"  => "int64_t",
            "System::Byte"   => "uint8_t",
            "System::UInt16" => "uint16_t",
            "System::UInt32" => "uint32_t",
            "System::UInt64" => "uint64_t",
            "System::Half"   => "_Float16",
            "System::Single" => "float",
            "System::Double" => "double",
            _                => "void*"
        };
    }

    public static string ToNativeInterfaceType(this ITypeSymbol type)
    {
        var name = type.GetFullName("::");
        return name switch
        {
            "System::SByte"  => "int8_t",
            "System::Int16"  => "int16_t",
            "System::Int32"  => "int32_t",
            "System::Int64"  => "int64_t",
            "System::Byte"   => "uint8_t",
            "System::UInt16" => "uint16_t",
            "System::UInt32" => "uint32_t",
            "System::UInt64" => "uint64_t",
            "System::Half"   => "_Float16",
            "System::Single" => "float",
            "System::Double" => "double",
            _               => $"{name}&"
        };
    }

    private static Diagnostic UnsupportedType(ITypeSymbol type)
        => Diagnostic.Create(
            "MLLIF01", "",
            $"'{GetFullName(type)}' is not supported by MLLIF",
            DiagnosticSeverity.Error, DiagnosticSeverity.Error,
            true, 0);

    public static string? MangleName(this IMethodSymbol method, SourceProductionContext? spc = null)
    {
        var builder = new StringBuilder();
        builder.Append("__mllif__").Append(method.Name);

        foreach (var param in method.Parameters)
        {
            if (param.Type.ToMangleCode() is not { } code)
            {
                spc?.ReportDiagnostic(UnsupportedType(param.Type));
                return null;
            }

            builder.Append('_').Append(code.Replace('*', 'p'));
        }

        return builder.ToString();
    }
}