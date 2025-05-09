using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFrontBuild.Diagnostics;

public class BindingDiagnostic(WorkspaceDiagnosticKind kind, string message, Project project, Document? document)
    : WorkspaceDiagnostic(kind, message)
{
    public Project   Project  { get; } = project;
    public Document? Document { get; } = document;

    public override string ToString()
    {
        var kindText = Kind switch
        {
            WorkspaceDiagnosticKind.Failure => "error",
            WorkspaceDiagnosticKind.Warning => "warning",
            _ => throw new ArgumentOutOfRangeException()
        };

        return $"{Document?.FilePath ?? Project.Name}: {kindText}: {Message}";
    }

    public static BindingDiagnostic Error(Project project, ISymbol symbol, string message)
    {
        return new BindingDiagnostic(WorkspaceDiagnosticKind.Failure, message, project, project.GetDocument(symbol));
    }
}