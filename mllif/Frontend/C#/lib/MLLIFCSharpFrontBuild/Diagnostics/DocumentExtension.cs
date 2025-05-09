using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFrontBuild.Diagnostics;

public static class DocumentExtension
{
    public static Document? GetDocument(this Project project, ISymbol symbol)
    {
        var syntax = symbol.DeclaringSyntaxReferences.FirstOrDefault()?.GetSyntax();
        return syntax is null ? null : project.GetDocument(syntax.SyntaxTree);
    }
}