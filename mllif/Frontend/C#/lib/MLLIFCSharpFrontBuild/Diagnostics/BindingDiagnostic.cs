using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFrontBuild.Diagnostics;

public class BindingDiagnostic(WorkspaceDiagnosticKind kind, string message, Project project, Document? document)
    : WorkspaceDiagnostic(kind, message)
{
    public Project   Project  { get; } = project;
    public Document? Document { get; } = document;

    public override string ToString()
    {
        var kindText = kind switch
        {
            WorkspaceDiagnosticKind.Failure => "error",
            WorkspaceDiagnosticKind.Warning => "warning",
            _ => throw new ArgumentOutOfRangeException()
        };

        return $"{kindText} ({Document?.FilePath ?? Project.Name}): {Message}";
    }
}