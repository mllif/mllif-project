using Microsoft.CodeAnalysis;

namespace MLLIFCSharpFrontBuild;

public interface IGenerator
{
    public IEnumerable<WorkspaceDiagnostic> Run(TextWriter writer, INamespaceSymbol symbol, Project project);
}